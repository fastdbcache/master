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
int PGStartupPacket3(int fd,PACK *pa){
    char *newbuf;
    size_t pv_len;
    uint32 pv;

    if(pa->pack == NULL) {
        d_log("pack calloc error !\n");
        return -1;
    }

    pv_len = Socket_Read(fd, pa->pack, sizeof(uint32));
    if(pv_len == sizeof(uint32)){
        memcpy(&pv, pa->pack, sizeof(uint32));

        pv = ntohl(pv);
        newbuf = realloc(pa->pack, pv);
        if(newbuf){
            pa->pack = newbuf;
        }else {
            printf("errr-----\n");
            return -1;
        }

        pv_len = Socket_Read(fd, pa->pack+sizeof(uint32), pv-sizeof(uint32));

        if(pv_len == (pv-sizeof(uint32))){
            /*uint32  v=0;
            memcpy(&v, pa->pack+sizeof(uint32), sizeof(uint32));
            printf("pv major: %d pv minor:%d\n", ntohl(v)>>16, ntohl(v)&0x0000ffff);
            printf("param: %s\n", pa->pack+sizeof(uint32)+sizeof(uint32));
            printf("pv:%d\n", pv);*/
            return pv;
        }
        
        return -1;
    }
    return -1;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  resolve_slot
 *  Description:  
 * =====================================================================================
 */
SESSION_SLOTS *resolve_slot(const char *buf){
    char *p;
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

        /*
         * From 9.0, the start up packet may include
         * application name. After receiving such that packet,
         * backend sends parameter status of application_name.
         * Upon reusing connection to backend, we need to
         * emulate this behavior of backend. So we remember
         * this and send parameter status packet to frontend
         * instead of backend in
         * connect_using_existing_connection().
         **/
        else if (!strcmp("application_name", p))
        {
            p += (strlen(p) + 1);
            _slot->application_name = p;
        }

        p += (strlen(p) + 1);
    }

    return _slot;
}		/* -----  end of function resolve_slot  ----- */


int AuthPG(const int bfd,const int ffd, SESSION_SLOTS *slot){
    char *newbuf,  *_apack, *_hdrtmp;
    ub1 *_drtmp;
    size_t totalsize,  total_size, cmd_size, pack_len;
    uint32 total;
    int rfd, wfd;
    MSGFORMAT *_mf;
    int type=1, isDATA;
    E_SQL_TYPE isSELECT;
    HDR *_hdr;
    ULIST *_ulist;
    SLABPACK *mem_pack;
    E_SQL_TYPE cache;
 
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
            _drtmp = (ub1 *)realloc(_hdr->dr, _hdr->drl+totalsize);    \
            if(_drtmp){                                         \
                _hdr->dr = _drtmp;                              \
                memcpy(_hdr->dr+_hdr->drl, _apack, totalsize);  \
                _hdr->drl += totalsize;                         \
            }else{                                              \
                freeHdr(_hdr);                                  \
                return -1;                                      \
            }                                                   \
        }                                                       \
    }while (0)

    FB(type);
      
    if(slot->backend_fd != 0) return -1;
    
    _hdr = NULL;
    _ulist = NULL;
    _apack = NULL;
    pack_len = 0;
    isSELECT = E_OTHER;
    isDATA = FALSE;

    FB(1);
    
    auth_loop:
        if(!_apack){
            _apack = calloc(1, sizeof(char));
            pack_len = sizeof(char);
        }
        bzero(_apack, pack_len);
        cmd_size = Socket_Read(rfd, _apack, sizeof(char));

        if(cmd_size != sizeof(char)) {            
            free(_apack);
            _apack = NULL;
            return -1;
        }
        if(pack_len < sizeof(char)+sizeof(uint32)){
            newbuf = realloc(_apack, sizeof(char)+sizeof(uint32));        
            pack_len += sizeof(uint32); 
            if(newbuf){
                _apack = newbuf;
            } else{
                free(_apack);
                _apack = NULL;
                return -1;
            }
        }
       /* DEBUG("ask: %c", *_apack);*/
        
        total_size = Socket_Read(rfd, _apack+sizeof(char), sizeof(uint32));

        if(total_size != sizeof(uint32)){
            free(_apack);
            _apack = NULL;
            return -1;
        }
        total = 0;
        memcpy(&total, _apack+sizeof(char), sizeof(uint32));

        total = ntohl(total);
        totalsize = sizeof(char)+total;
        if(pack_len < totalsize){
            
            newbuf = realloc(_apack, totalsize);
            pack_len = totalsize;

            if(newbuf){
                _apack = newbuf;
            } else{
                free(_apack);
                _apack = NULL;
                return -1;
            }
        }
        Socket_Read(rfd, _apack+sizeof(char)+sizeof(uint32), total-sizeof(uint32));

        if(*_apack != 'Q')
            Socket_Send(wfd, _apack, totalsize);

        if ( slot->backend_fd == 0 && *_apack != 'p') {
            _mf = (MSGFORMAT *)calloc(1, sizeof(MSGFORMAT));
            _mf->format = (char *)calloc(1, totalsize);

            memcpy(_mf->format, _apack, totalsize);                    
            _mf->format_len = totalsize;
           
            if(slot->tail == NULL){
                slot->tail = slot->head = _mf;

            }else{    
                slot->head->next = _mf;
                slot->head = _mf;
            }   
        }

        switch ( *_apack ) {
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
                _hdrtmp = _apack+sizeof(char)+sizeof(uint32);
                isSELECT = findSQL(_hdrtmp, total-sizeof(uint32));
                if(isSELECT == E_SELECT){
                    mem_pack = (SLABPACK *)calloc(1, sizeof(SLABPACK));
                        
                    hkey(_hdrtmp, total-sizeof(uint32), mem_pack);
                    
                    if(mem_pack->len > 0){                        
                        Socket_Send(rfd, mem_pack->pack, mem_pack->len);
                        /*free(mem_pack->pack);*/
                        mem_pack->pack = NULL;
                        FB(0);
                        goto free_pack;

                    }else{
                        
                        _hdr = hdrcreate(); 
                        _hdr->keyl = total-sizeof(uint32);
                        _hdr->key = (ub1 *)calloc(_hdr->keyl, sizeof(ub1));
                        memcpy(_hdr->key, _hdrtmp, _hdr->keyl);
                        _hdr->stime = get_sec(); 
                        _hdr->flag = H_TRUE;                    
                    }
                    
                    free(mem_pack);
                }else if(isSELECT==E_DELETE || isSELECT==E_UPDATE || isSELECT==E_INSERT){
                    _ulist = initulist();
                    if(_ulist){
                        _ulist->keyl = total-sizeof(uint32);
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
                        getItemStat(_hdrtmp+clen, total-sizeof(uint32)-clen , rfd);
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
                    }
                    
                }
                /*  else{
                    DEBUG("system table");
                }*/
                Socket_Send(wfd, _apack, totalsize);
                FB(1);
                goto free_pack;
            case 'T':
                FB(1);
                                            
                if(isSELECT == E_SELECT && _hdr){
                    _hdr->dr = (ub1 *)calloc(totalsize, sizeof(ub1));
                    
                    memcpy(_hdr->dr, _apack, totalsize);
                    _hdr->drl = totalsize;
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
                free(_apack);                
                _apack = NULL;
                pack_len = 0;
                isDATA = FALSE;
                return 0;
                
            default:	
                printf("any:%c\n", *_apack);
                break;
        }				/* -----  end switch  ----- */
    
    free_pack:        
        goto auth_loop;

    if(_apack)free(_apack);
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

    while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n'){  
        p++;          
    } 
    
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
    const char *p = sql;

    
    while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n'){  
        (*offset)++;
        p += (*offset);
    }
    (*offset) += 5;
    p += 5;
    if(*p != ' '){
         return E_OTHER;
    }
    while(*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n'){
        (*offset)++;
        p++;
    }
    if(memcmp(p, item, strlen(item)) == 0){
        (*offset) += strlen(item);
        p += strlen(item);

        if(*p != ' '){
            return E_OTHER;
        }
        while(*p == ' '){
            (*offset)++;
            p++;
        }
        return E_CACHE_ITEM;
    }
    else if(memcmp(p, version, strlen(version)) == 0){
        p += strlen(version);
        
        while(*p == ' '){
            p++;
        }
        if(*p != ';') return E_OTHER;

        return E_CACHE_VERSION;
    }else if(memcmp(p, htab, strlen(htab)) == 0){
        p += strlen(htab);
        while(*p == ' '){
            p++;
        }
        if(*p != ';') return E_OTHER;

        return E_CACHE_STAT;
    }else if(memcmp(p, helps, strlen(helps)) == 0){
        p += strlen(helps);
        while(*p == ' '){
            p++;
        }
        if(*p != ';') return E_OTHER;

        return E_CACHE_HELP;
    }
    
    return E_OTHER;
}		/* -----  end of function findCache  ----- */
/* vim: set ts=4 sw=4: */

