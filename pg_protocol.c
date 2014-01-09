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

#include "pg_protocol.h"


/* 
 * client to pg
 * */
int PGStartupPacket3(int fd, DBP *_dbp){
    char *newbuf;
    size_t pv_len;
    uint32 pv;
    int res;

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


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  resolve_slot
 *  Description:  
 * =====================================================================================
 
SESSION_SLOTS *resolve_slot(const char *buf){
    const char *p;
    SESSION_SLOTS *_slot;
    uint32 v;

    if(buf == NULL)return NULL;
    p = buf; 

    _slot = (SESSION_SLOTS *)calloc(1, sizeof(SESSION_SLOTS));
    _slot->backend_fd = 0;

    memcpy(&v, p+sizeof(uint32), sizeof(uint32));
    _slot->major = ntohl(v)>>16;
    _slot->minor = ntohl(v)&0x0000ffff;

    p +=  sizeof(uint32)+sizeof(uint32);

    while(*p)
    {
        if (!strcmp("user", p))
        {
            p += (strlen(p) + 1);
            _slot->user = strdup(p);
                
            if (!_slot->user)
            {
                free(_slot);
                return NULL;
            }
        }
        else if (!strcmp("database", p))
        {
            p += (strlen(p) + 1);
            _slot->database = strdup(p);
            if (!_slot->database)
            {
                free(_slot->user);
                free(_slot);
                return NULL;
            }
        }
        
        else if (!strcmp("application_name", p))
        {
            p += (strlen(p) + 1);
            _slot->application_name = p;
        }

        p += (strlen(p) + 1);
    }

    return _slot;
}	*/	/* -----  end of function resolve_slot  ----- */


int AuthPG(const int bfd,const int ffd){
    char *newbuf,  *_hdrtmp;
    DBP *_apack, *depo_pack; 
    ub1 *_drtmp;
    size_t totalsize,  total_size, cmd_size, pack_len;
    uint32 total;
    int rfd, wfd;
    int type, isDATA;
    E_SQL_TYPE isSELECT;
    HDR *_hdr;
    ULIST *_ulist;
    SLABPACK *mem_pack;
    E_SQL_TYPE cache;
    _ly *ply;
    int isDep;
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
        if(isSELECT == E_SELECT && _hdr){                    \
            _drtmp = (ub1 *)realloc(_hdr->dr, _hdr->drl+_apack->inEnd);    \
            if(_drtmp){                                         \
                _hdr->dr = _drtmp;                              \
                memcpy(_hdr->dr+_hdr->drl, _apack->inBuf, _apack->inEnd);  \
                _hdr->drl += _apack->inEnd;                         \
            }else{                                              \
                freeHdr(_hdr);                                  \
                return -1;                                      \
            }                                                   \
        }                                                       \
    }while (0)
            
    _hdr = NULL;
    _ulist = NULL;
    _apack = initdbp();
    pack_len = 0;
    isSELECT = E_OTHER;
    isDATA = FALSE;
    depo_lock = H_FALSE;
     
    FB(1);
    
    auth_loop:
        _apack->inCursor = 0;
                
        if(!_apack->inBuf){
            
            if(CheckBufSpace(sizeof(char), _apack)!=0)return -1;

        }else _apack->inEnd = sizeof(char);
        
        bzero(_apack->inBuf, _apack->inBufSize);

        cmd_size = Socket_Read(rfd, _apack->inBuf, sizeof(char));

        if(cmd_size != sizeof(char)) {            
            freedbp(_apack); 
            return -1;
        }
        /*  DEBUG("ask:%c", *(_apack->inBuf));  */
        if(CheckBufSpace(sizeof(uint32), _apack) != 0){
            return -1;
        }
                    
        total_size = Socket_Read(rfd, _apack->inBuf + _apack->inCursor, sizeof(uint32));

        if(total_size != sizeof(uint32)){
            freedbp(_apack);
            return -1;
        }
        total = 0;
        
        getInt(&total, 4, _apack);
        
        totalsize = total-sizeof(uint32);
        if(CheckBufSpace(totalsize, _apack) != 0)return -1;
                         
        Socket_Read(rfd, _apack->inBuf+_apack->inCursor, totalsize);

        if(*_apack->inBuf == 'X' &&
            conn_global->hasdep == H_TRUE){
            RQ_BUSY(isDep);

            if(isDep < conn_global->quotient &&
                pools_dest->doing == H_FALSE){
                DEP_DO_LOCK();
                if(pools_dest->doing == H_FALSE){
                    pools_dest->doing = H_TRUE;
                    depo_lock = H_TRUE;                    
                }
                DEP_DO_UNLOCK();                
            }
        }

        if(depo_lock == H_TRUE){
            if(*_apack->inBuf == 'C' |
                *_apack->inBuf == 'E')
                goto free_pack;

            if(!depo_pack)
                depo_pack = initdbp();

            if(leadpush(depo_pack) == -1){
                pools_dest->doing = H_FALSE;
                depo_pack = H_FALSE;
                leadexit(depo_pack);
            }
            DEBUG("depo_sql:%s", depo_pack->inBuf+sizeof(char)+sizeof(uint32));
            Socket_Send(bfd, depo_pack->inBuf, depo_pack->inEnd);
            FB(1);
            goto free_pack;
        }

        if(*_apack->inBuf != 'Q' ){
            
            if(Socket_Send(wfd, _apack->inBuf, _apack->inEnd) != _apack->inEnd){
                DEBUG("error");
            }
        }
        

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
                    mem_pack = (SLABPACK *)calloc(1, sizeof(SLABPACK));
                        
                    hkey(_hdrtmp,_apack->inEnd-_apack->inCursor , mem_pack);
                    
                    if(mem_pack->len > 0){                        
                        Socket_Send(rfd, mem_pack->pack, mem_pack->len);
                        /*free(mem_pack->pack);*/
                        mem_pack->pack = NULL;
                        FB(0);
                        goto free_pack;

                    }else{
                        
                        _hdr = hdrcreate(); 
                        _hdr->keyl = _apack->inEnd-_apack->inCursor;
                        _hdr->key = (ub1 *)calloc(_hdr->keyl, sizeof(ub1));
                        memcpy(_hdr->key, _hdrtmp, _hdr->keyl);
                        _hdr->stime = get_sec(); 
                    }
                    
                    free(mem_pack);
                }else if(isSELECT==E_DELETE || isSELECT==E_UPDATE || isSELECT==E_INSERT){
                    DEBUG("deprule:%s", conn_global->deprule); 
                    if(conn_global->hasdep == H_TRUE &&
                        conn_global->deprule){
                        RQ_BUSY(isDep);
                        DEBUG("isDep:%d", isDep);
                        if(isDep > conn_global->quotient){                        
                            ply = parser_do (_hdrtmp, _apack->inEnd-_apack->inCursor);
                            if(ply){ 
                                /*  DEPR *_depr;
                                for(_depr = conn_global->deprule; _depr; _depr=_depr->next){
                                    if(_depr->len == ply->len &&
                                        !memcmp(_depr->table, ply->tab, ply->len)){
                                 */
                                DEBUG("tab:%s", ply->tab);
                                if(strstr(conn_global->deprule, ply->tab)){
                                        leadadd ( (ub1)_apack->inBuf, (ub4)_apack->inEnd );        
                                    
                                }
                                free(ply->tab);
                                free(ply);
                                FB(0);
                                goto free_pack;
                            }
                        }
                    }
                    _ulist = initulist();
                    if(_ulist){
                        _ulist->keyl = _apack->inEnd-_apack->inCursor;
                        _ulist->key = calloc(_ulist->keyl, sizeof(char));
                        if(_ulist->key){
                            memcpy(_ulist->key, _hdrtmp, _ulist->keyl);
                            _ulist->utime = get_sec();
                        }else{
                            freeUList(_ulist);
                            _ulist = NULL;
                        }
                    }
                }else if(isSELECT == E_CACHE){
                    /*  listHslab();
                    setCacheRowDescriptions(rfd);*/
                    int clen=0; 
                    cache = findCache(_hdrtmp, &clen);
                    if(cache == E_CACHE_ITEM){
                        DEBUG("sql:%s, len:%d", _hdrtmp+clen, _apack->inEnd-_apack->inCursor-clen);
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
                        ssize_t set_val;
                        int set_offset;
                        _hdrtmp+=clen;
                        SPACER(_hdrtmp, set_offset); 
                        fdbcSet(rfd);
                        FB(0);
                        goto free_pack;
                    }
                    
                }
                /*  else{
                    DEBUG("system table");
                }*/
                Socket_Send(wfd, _apack->inBuf, _apack->inEnd );
                FB(1);
                goto free_pack;
            case 'T':
                FB(1);
                                            
                if(isSELECT == E_SELECT && _hdr){
                    _hdr->dr = (ub1 *)calloc(_apack->inEnd, sizeof(ub1));
                    
                    memcpy(_hdr->dr, _apack->inBuf, _apack->inEnd);
                    _hdr->drl = _apack->inEnd;
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
                /*    DEBUG("C:%s", _apack+sizeof(char)+sizeof(uint32));*/
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
                if(_ulist)
                    addUlist(_ulist);                 
                _hdr = NULL;
                _ulist = NULL;
                /*  DEBUG("Z:%s", _apack+sizeof(char)+sizeof(uint32));*/
                goto free_pack;
            case 'X':
                
                pack_len = 0;
                freedbp(_apack);
                isDATA = FALSE;
                return 0;
                
            default:	
                printf("any:%c\n", *_apack->inBuf);
                break;
        }				/* -----  end switch  ----- */
    
    free_pack:        
        goto auth_loop;

    freedbp(_apack); 
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

