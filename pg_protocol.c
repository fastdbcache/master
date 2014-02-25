/*
 * =====================================================================================
 *
 *       Filename:  pg-protocol3.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/05/2013 10:42:44 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), 
 *   Organization:  
 *
 * =====================================================================================
 */
/*    Copyright 2009 10gen Inc.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
#include "pg_protocol.h"


/* 
 * client to pg
 * */
int PGStartupPacket3(int fd, DBP *_dbp){
    size_t pv_len;
    int pv;
    int res;

    _dbp->inCursor = 0; 
    _dbp->inEnd = 0;
    if(_dbp == NULL) {
        d_log("pack calloc error !\n");         
        return -1;
    }
    res = CheckBufSpace(sizeof(uint32), _dbp);
    if(res == -1) {
        DEBUG("CheckBufSpace error !\n");
        return -1;
    }
    
    pv_len = Socket_Read(fd, _dbp->inBuf, sizeof(uint32));

    if(pv_len != sizeof(uint32)) return -1;

    getInt(&pv, 4, _dbp);
    res = CheckBufSpace((pv-sizeof(uint32)), _dbp);
    if(res == -1) return -1;

    pv_len = Socket_Read(fd, _dbp->inBuf+_dbp->inCursor, pv-sizeof(uint32));

    if(pv_len == (pv-sizeof(uint32))){
        /*uint32  v=0;
        memcpy(&v, _dbp->inBuf+sizeof(uint32), sizeof(uint32));
        printf("pv major: %d pv minor:%d\n", ntohl(v)>>16, ntohl(v)&0x0000ffff);
        printf("param: %s\n", _dbp->inBuf+sizeof(uint32)+sizeof(uint32));
        printf("pv:%d\n", pv);  */
        return pv;
    }
    
    return -1;
     
}

int AuthPG(const int bfd,const int ffd, DBP *_dbp){
    char *_hdrtmp;
    DBP *_apack, *depo_pack; 
    ub1 *_drtmp;
    size_t totalsize,  total_size, cmd_size, pack_len;
    int total, lead_res;
    int rfd, wfd;
    int  isDATA;
    E_SQL_TYPE isSELECT;
    HDR *_hdr;
    ULIST *_ulist;
    SLABPACK *mem_pack;
    E_SQL_TYPE cache;
    _ly *ply;
    int isDep ;
    H_STATE depo_lock;

    #define FB(type) \
	do \
	{ \
        if((type)==0){  \
            rfd = ffd;  \
            wfd = bfd;  \
        }else{          \
            rfd = bfd;   \
            wfd = ffd;  \
        }             \
	} while (0)

    #define STORE()         \
    do                      \
    {                           \
        if(isSELECT == E_SELECT &&  \
            _hdr &&                   \
            _apack->inEnd > 1){                    \
            _drtmp = (ub1 *)realloc(_hdr->dr, _hdr->drl+_apack->inEnd);    \
            if(_drtmp){                                         \
                _hdr->dr = _drtmp;                              \
                memcpy(_hdr->dr+_hdr->drl, _apack->inBuf, _apack->inEnd);  \
                _hdr->drl += _apack->inEnd;                         \
            }else{                                              \
                freeHdr(_hdr);                                  \
                _hdr = NULL;                                      \
            }                                                   \
        }                                                       \
    }while (0)
            
    _hdr = NULL;
    _ulist = NULL;
    _apack = _dbp;
    pack_len = 0;
    isSELECT = E_OTHER;
    isDATA = FALSE;
    depo_lock = H_FALSE;
    depo_pack = NULL; 
    isDep = 0;
    FB(1);
    
    auth_loop:
        
        _apack->inCursor = 0;
         
        if(!_apack->inBuf){
            
            if(CheckBufSpace(sizeof(char), _apack)!=0){
                DEBUG("CheckBufSpace error");
                return -1;
            }

        }else _apack->inEnd = sizeof(char);
        bzero(_apack->inBuf, _apack->inBufSize);
        cmd_size = Socket_Read(rfd, _apack->inBuf, sizeof(char));
        if(cmd_size != sizeof(char)) {            
            return -1;
        }
         
        if(CheckBufSpace(sizeof(uint32), _apack) != 0){
            DEBUG("CheckBufSpace error");
            return -1;
        }
        total_size = Socket_Read(rfd, _apack->inBuf + _apack->inCursor, sizeof(uint32));
        if(total_size != sizeof(uint32)){
            DEBUG("total_size error");
            return -1;
        }
        total = 0;
        
        getInt(&total, 4, _apack);
        
        totalsize = total-sizeof(uint32);
        if(CheckBufSpace(totalsize, _apack) != 0){
            DEBUG("CheckBufSpace error");
            return -1;
        }
        Socket_Read(rfd, _apack->inBuf+_apack->inCursor, totalsize);

        if(*_apack->inBuf == 'X' &&
            conn_global->hasdep == H_TRUE){
             
            if(pools_dest->isfull == H_TRUE){                
                isDep = conn_global->quotient-1;                 
            }else{
                RQ_BUSY(isDep);
                       
            }
            if(isDep <= conn_global->quotient &&
                pools_dest->doing == H_FALSE){
                DEP_DO_LOCK();                
                if(pools_dest->doing == H_FALSE){
                    pools_dest->doing = H_TRUE;
                    depo_lock = H_TRUE;                    
                }else{
                    DEP_DO_UNLOCK();
                    return -1;
                }
                DEP_DO_UNLOCK();                
            }
             else{
                /*DEBUG("free isDep:%d, quotient:%d", isDep, conn_global->quotient);  */
                return -1;
            } 
            
        }
        
        if(depo_lock == H_TRUE){
            if(*_apack->inBuf == 'C' ||
                *_apack->inBuf == 'T' ||
                *_apack->inBuf == 'E' ||
                *_apack->inBuf == 'D'){
                goto free_pack;
            }
            if(!depo_pack){
                depo_pack = initdbp();
            }
            depo_pack->inEnd = 0; 
            lead_res = leadpush(depo_pack);
            if(lead_res == -1){
                pools_dest->doing = H_FALSE;
                depo_lock = H_FALSE;
                if(conn_global->deptype == D_MEM){
                    depo_pack->inBuf = NULL;
                    depo_pack->inBufSize = 0;
                }
                depo_pack->inEnd = 0;
                leadexit(depo_pack);               
            }
            
            Socket_Send(bfd, depo_pack->inBuf, depo_pack->inEnd);
            if(*depo_pack->inBuf == 'X'){
                
                freedbp(depo_pack);
                return -1;
            }
            FB(1);
            goto free_pack;
        }
        if(*_apack->inBuf != 'Q'){
            
            if(Socket_Send(wfd, _apack->inBuf, _apack->inEnd) != _apack->inEnd){
                DEBUG("error");
            }
        }
        /*DEBUG("1.ask:%c", *(_apack->inBuf));  */
        switch ( *_apack->inBuf ) {
            case 'R':	
                /* total eq 8 is AuthenticationOk */
                if(total==8){
                    FB(1);
                }else{
                    FB(0);
                }
                
                goto free_pack;

            case 'p':	
                FB(1);
                goto free_pack;
            case 'S':
                FB(1);
                goto free_pack;
            case 'K':
                FB(1);
                goto free_pack;
            
            case 'E':                
                
                if(_hdr){
                    freeHdr(_hdr);
                    _hdr = NULL;
                }
                if(_ulist){
                    freeUList(_ulist);
                    _ulist = NULL;
                }

                goto free_pack; 
            case 'Q':                
                isDATA = FALSE;                
                _hdrtmp = _apack->inBuf + _apack->inCursor;
                isSELECT = findSQL(_hdrtmp, _apack->inEnd-_apack->inCursor);
                if(isSELECT == E_SELECT){
                    GET_LOCK();
                    pools_htab->get++;
                    GET_UNLOCK();
                    checkLimit(_apack);
                    _hdrtmp = _apack->inBuf + _apack->inCursor;
                    mem_pack = (SLABPACK *)calloc(1, sizeof(SLABPACK));
                    if(mem_pack){ 
                        mem_pack->len = 0;
                        hkey(_hdrtmp,_apack->inEnd-_apack->inCursor , mem_pack);
                        if(mem_pack->len > 0){
                            Socket_Send(rfd, mem_pack->pack, mem_pack->len);                        
                            /*
                            free(mem_pack->pack);*/
                            mem_pack->pack = NULL;
                            FB(0);
                            free(mem_pack);
                            goto free_pack;
                        
                        }else{
                            _hdr = hdrcreate(); 
                            if(_hdr && (_apack->inEnd-_apack->inCursor)<KEY_LENGTH ){
                                _hdr->keyl = _apack->inEnd-_apack->inCursor;
                                memcpy(_hdr->key, _hdrtmp, _hdr->keyl);
                                _hdr->stime = get_sec(); 
                            }else{
                                free(_hdr);
                                _hdr = NULL;
                            }
                        }
                        
                        free(mem_pack);
                    }
                }else if(isSELECT==E_DELETE || isSELECT==E_UPDATE || isSELECT==E_INSERT){
                    SET_LOCK();
                    pools_htab->set++;
                    SET_UNLOCK();
                    ply = parser_do (_hdrtmp, _apack->inEnd-_apack->inCursor);
                    _ulist = initulist();
                    if(_ulist &&
                        ply){
                        _ulist->keyl = ply->len;
                        if(_ulist->keyl < (KEY_LENGTH-1)){
                            memcpy(_ulist->key, ply->tab, _ulist->keyl);
                            _ulist->utime = get_sec();
                        }else{
                            freeUList(_ulist);
                            _ulist = NULL;
                        }
                    }
                    if(ply){
                        if(conn_global->hasdep == H_TRUE &&
                            conn_global->deprule){
                       
                            RQ_BUSY(isDep);
                            /*DEBUG("add isDep:%d, quotient:%d", isDep, conn_global->quotient);  */
                            if(isDep > conn_global->quotient){
                                      
                                if(memmem(conn_global->deprule, strlen(conn_global->deprule), ply->tab, ply->len)!=NULL){
                                    if(-1 == leadadd ( (ub1 *)_apack->inBuf, (ub4)_apack->inEnd)){
                                        goto leaderr;
                                    }                                    
                                    int ii;
                                    if(isSELECT == E_DELETE){
                                        ii = 3; 
                                    }else if(isSELECT == E_UPDATE){
                                        ii = 1;
                                    }else if(isSELECT == E_INSERT){
                                        ii = 2;
                                    }
                                        
                                    CommandComplete(ii, 1 , rfd);
                                    ReadyForQuery(rfd);

                                    FB(0);
                                    free(ply->tab);
                                    free(ply);
                                    goto free_pack;
                                }                                                                                            
                            }
                        }
                    }else DEBUG("ply error len:%d :%s",_apack->inEnd-_apack->inCursor, _hdrtmp);

                    leaderr:                                         
                        if(ply){
                            free(ply->tab);
                            free(ply);
                        }
                }else if(isSELECT == E_CACHE){
                    /*  listHslab();
                    setCacheRowDescriptions(rfd);*/
                    int clen=0; 
                    cache = findCache(_hdrtmp, &clen);
                    if(cache == E_CACHE_ITEM){
                        getItemStat(_hdrtmp+clen, _apack->inEnd-_apack->inCursor-clen , rfd);
                        FB(0);
                        goto free_pack;
                    }else if(cache == E_CACHE_VERSION){
                        getVer(rfd); 
                        FB(0);
                        goto free_pack;
                    }else if(cache == E_CACHE_STAT){
                        
                        gethtabstat(rfd); 
                        FB(0);
                        goto free_pack;
                    }else if(cache == E_CACHE_HELP){
                        
                        fdbcHelp(rfd); 
                        FB(0);
                        goto free_pack;
                    }else if(cache == E_CACHE_SET){
                        int set_offset;
                        _hdrtmp+=clen;
                        SPACER(_hdrtmp, set_offset); 
                        fdbcSet(rfd);
                        FB(0);
                        goto free_pack;
                    }
                    
                }/*
                  else{
                    DEBUG("system table:%s, len:%d", _apack->inBuf+sizeof(char)+sizeof(uint32), _apack->inEnd);
                }    */
                Socket_Send(wfd, _apack->inBuf, _apack->inEnd );
                FB(1);
                goto free_pack;
            case 'T':
                FB(1);
                                            
                if(isSELECT == E_SELECT && _hdr){
                    _hdr->dr = (ub1 *)calloc(_apack->inEnd, sizeof(ub1));
                    if(_hdr->dr){ 
                        memcpy(_hdr->dr, _apack->inBuf, _apack->inEnd);
                        _hdr->drl = _apack->inEnd;
                    }else{
                        freeHdr(_hdr);
                        _hdr = NULL;
                    }
                }
                goto free_pack;
            case 'D':
                FB(1);
                isDATA = TRUE;
                STORE();
                
                goto free_pack;
            case 'C':
                FB(1);

                STORE(); 
                goto free_pack;
            case 'Z':                
                FB(0);
                STORE();
                if(isSELECT == E_SELECT 
                    && _hdr
                    && isDATA == TRUE){
                    if(_hdr->drl <= LIMIT_SLAB_BYTE ){                        
                        
                        addHdr(_hdr);
                    }
                }else{
                    freeHdr(_hdr);
                }
                isDATA = FALSE;
                if(_ulist){
                    addUlist(_ulist);
                }
                _hdr = NULL;
                _ulist = NULL;
                goto free_pack;
            case 'X':
                
                pack_len = 0;
                /* freedbp(_apack); */
                isDATA = FALSE;
                return 0;
                
            default:
                DEBUG("any:%c",*_apack->inBuf);	
                return 0;
        }				/* -----  end switch  ----- */
    
    free_pack:        
        goto auth_loop;

    /*freedbp(_apack);   */
    _apack = NULL;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  findSQL
 *  Description:  
 * =====================================================================================
 */
E_SQL_TYPE findSQL (  const char *sql, int len ){
    const char *p = sql;
    char s[] = "select", in[] = "insert", u[] = "update", d[] = "delete", cache[] = "cache";
    int i;

    #define TYPE(var, type) \
	do \
	{ \
        for(i=0; i<strlen(var); i++, p++){  \
            if(tolower(*p) != var[i]){      \
                return E_OTHER;             \
            }                               \
        }                                   \
        return type;                        \
	} while (0)

    if(len < strlen(s)) return -1;

    SPACE(p);
    if(memcmp(p, cache, strlen(cache)) == 0){                
        return E_CACHE;
    }
          
    if(tolower(*p)== s[0]){
        for(i=0; Query_for_list[i]!=NULL; i++){
            if(strstr(sql, Query_for_list[i])!=NULL)return E_OTHER;
        }

        TYPE(s, E_SELECT);

    }else if(tolower(*p) == in[0]){
        TYPE(in, E_INSERT);	

    }else if(tolower(*p) == u[0]){
        TYPE(u, E_UPDATE);
    
    }else if(tolower(*p) == d[0]){
        TYPE(d, E_DELETE);
        
    }else{
        return E_OTHER;
    }				/* -----  end switch  ----- */


    return E_OTHER;
}		/* -----  end of function findSQL  ----- */



/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  findCache
 *  Description:  
 * =====================================================================================
 */
E_SQL_TYPE findCache (const char *sql, int *offset){
    char item[]="item";
    char version[]="version";
    char htab[]="stat";
    char helps[]="help";
    char sets[]="set";
    const char *p = sql;

    SPACER(p, (*offset));

    (*offset) += 5;
    p += 5;
    if(*p != ' '){
         return E_OTHER;
    }

    SPACER(p, (*offset));
    
    if(memcmp(p, item, strlen(item)) == 0){
        (*offset) += strlen(item);
        p += strlen(item);

        if(*p != ' '){
            return E_OTHER;
        }
        SPACER(p, (*offset));    
   
        return E_CACHE_ITEM;
    }else if(memcmp(p, sets, strlen(sets)) == 0){
        (*offset) += strlen(sets);
        p += strlen(sets);

        if(*p != ' '){
            return E_OTHER;
        }
        SPACER(p, (*offset));
    
        return E_CACHE_SET;
    }
    else if(memcmp(p, version, strlen(version)) == 0){
        p += strlen(version);
  
        SPACE(p);

        if(*p != ';') return E_OTHER;

        return E_CACHE_VERSION;
    }else if(memcmp(p, htab, strlen(htab)) == 0){
        p += strlen(htab);
        SPACE(p);
 
        if(*p != ';') return E_OTHER;

        return E_CACHE_STAT;
    }else if(memcmp(p, helps, strlen(helps)) == 0){
        p += strlen(helps);

        SPACE(p);
        if(*p != ';') return E_OTHER;

        return E_CACHE_HELP;
    }
    
    return E_OTHER;
}		/* -----  end of function findCache  ----- */

/* vim: set ts=4 sw=4: */

