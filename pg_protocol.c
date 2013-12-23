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
    uint32 pv, v;

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
            /*  v=0;
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


int AuthPG(const int bfd,const int ffd, SESSION_SLOTS *slot, ssize_t no){
    char *newbuf,  *_apack, *_drtmp, *_hdrtmp;
    size_t totalsize, offset,  total_size, cmd_size;
    uint32 total;
    int rfd, wfd, q=0;
    MSGFORMAT *_mf;
    int type=1, isSELECT, mem_len;
    HDR *_hdr;
    ULIST *_ulist;
    SLABPACK *mem_pack;
    
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
    {                                           \ 
        if(isSELECT == 0 && _hdr){                    \
            _drtmp = realloc(_hdr->dr, _hdr->drl+totalsize);    \
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
    isSELECT = -1;

    FB(1);
    
    auth_loop:
        _apack = calloc(1, sizeof(char));
        cmd_size = Socket_Read(rfd, _apack, sizeof(char));

        if(cmd_size != sizeof(char)) {            
            free(_apack);
            _apack = NULL;
            return -1;
        }

        newbuf = realloc(_apack, sizeof(char)+sizeof(uint32));
        
        if(newbuf){
            _apack = newbuf;
        } else{
            free(_apack);
            _apack = NULL;
            return -1;
        }
        /* printf("ask: %c\n", *_apack);*/

        total_size = Socket_Read(rfd, _apack+sizeof(char), sizeof(uint32));

        if(total_size != sizeof(uint32)){
            free(_apack);
            _apack = NULL;
            return -1;
        }
        total = 0;
        memcpy(&total, _apack+sizeof(char), sizeof(uint32));

        total = ntohl(total);

        //if(totalsize < total){
            totalsize = sizeof(char)+total;
            newbuf = realloc(_apack, total+sizeof(char));
        
            if(newbuf){
                _apack = newbuf;
            } else{
                free(_apack);
                _apack = NULL;
                return -1;
            }
        //}
        Socket_Read(rfd, _apack+sizeof(char)+sizeof(uint32), total-sizeof(uint32));

        Socket_Send(wfd, _apack, totalsize);

        if ( slot->backend_fd == 0 && *_apack != 'p') {
            MSGFORMAT *_mf;
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

                if(total==8){
                    FB(1);
                }else
                    FB(0);
                
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
                printf("E\n");

                if(_hdr){
                    freeHdr(_hdr);
                    _hdr = NULL;
                }
                if(_ulist){
                    freeUList(_ulist);
                    _ulist = NULL;
                }

                return -1; 
            case 'Q':
                FB(1);

                _hdrtmp = _apack+sizeof(char)+sizeof(uint32);                    
                isSELECT = findSQL(_hdrtmp, total);
                if(isSELECT == 0){
                    mem_pack = (SLABPACK *)calloc(1, sizeof(SLABPACK));
                        
                    hkey(_hdrtmp, total, mem_pack);
                    
                    if(mem_pack->len > 0){                        
                        Socket_Send(wfd, mem_pack->pack, mem_pack->len);
                        free(mem_pack->pack);
                        
                        FB(0);
                    }else{
                        _hdr = hdrcreate(); 
                        _hdr->keyl = total;
                        _hdr->key = (char *)calloc(total, sizeof(char));
                        memcpy(_hdr->key, _hdrtmp, total);
                        _hdr->stime = get_sec(); 
                        _hdr->flag = H_TRUE;                    
                    }

                    free(mem_pack);
                }else if(isSELECT == -1){
                    _ulist = (ULIST *)calloc(1, sizeof(ULIST));
                    _ulist->keyl = total;
                    _ulist->key = calloc(total, sizeof(char));
                    memcpy(_ulist->key, _hdrtmp, total);
                    _ulist->utime = get_sec();
                    _ulist->flag = H_TRUE;

                }
                /*else{
                    DEBUG("system table");
                }*/

                goto free_pack;
            case 'T':
                FB(1);
                if(isSELECT == 0 && _hdr){
                    _hdr->dr = (char *)calloc(totalsize, sizeof(char));
                    memcpy(_hdr->dr, _apack, totalsize);
                    _hdr->drl = totalsize;
                }

                goto free_pack;
            case 'D':
                FB(1);
                STORE();

                goto free_pack;
            case 'C':
                FB(1);
                STORE(); 
                goto free_pack;
            case 'Z':
                FB(0);
                STORE();

                if(isSELECT == 0 && _hdr){
                    addHdr(_hdr, no);
                }
                if(_ulist)
                    addUlist(_ulist, no);
                goto free_pack;
            case 'X':
                free(_apack);                
                _apack = NULL;
                return 0;
                
            default:	
                //printf("any:%c\n", *_apack);
                break;
        }				/* -----  end switch  ----- */
    
    free_pack:
        if(_apack)free(_apack);
        _apack = NULL;
        goto auth_loop;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  findSQL
 *  Description:  
 * =====================================================================================
 */
int findSQL (  const char *sql, int len ){
    const char *p = sql;
    char u[] = "SELECT";
    char l[] = "select";
    int i;

    if(len < strlen(l)) return -1;

    while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n'){  
        p++;          
    } 
    
    for(i=0; i<strlen(l); i++, p++){
        if(*p != u[i] && *p != l[i]){
            return -1;
        }       
    }
    
    for(i=0; Query_for_list[i]!=NULL; i++){
        if(strstr(sql, Query_for_list[i])!=NULL)return -2;
    }

    return 0;
}		/* -----  end of function findSQL  ----- */

int PGExchange(const int bfd,const int ffd, SESSION_SLOTS *slot){
    char *newbuf, ask, *pack;
    size_t psize, totalsize, offset, cmd_size, total_size, name_size, encry_size;
    uint32 total, encry;
    int rfd, wfd, q=0;
    MSGFORMAT *_mf;
    int type=1;
    
    /* if(cache_hast_table == NULL){
        cache_hast_table = hcreate(8);
    }*/

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

    FB(0);
    
    pack = calloc(1, sizeof(uint32));
    totalsize = sizeof(uint32);
    for(;;){
        psize = 0;
        
        /*  if(newbuf){
                pack = newbuf;
        } else return -1;*/
        //printf("pro start--\n");
        if(type == 1)
        cmd_size = Socket_Read(rfd, &ask, sizeof(char));
        else
        cmd_size = Socket_Read(rfd, &ask, sizeof(char));

        if(cmd_size != sizeof(char)) {            
            //printf("error cmd_size:%d ask %c \n",cmd_size, ask);
            free(pack);
            return -1;
        }

        if(ask == 'X')return 0;
       // printf("ask: %c\n", ask);
        if(type == 1)    
        total_size = Socket_Read(rfd, pack+psize, sizeof(uint32));
        else
        total_size = Socket_Read(rfd, pack+psize, sizeof(uint32));

        if(total_size != sizeof(uint32)) return -1;
        memcpy(&total, pack, sizeof(uint32));

        total = ntohl(total);
        psize += total;

        if(totalsize < total){
            totalsize = total;
            newbuf = realloc(pack, totalsize);
        
            if(newbuf){
                    pack = newbuf;
            } else return -1;
        }
        if(type == 1)
        name_size = Socket_Read(rfd, pack+sizeof(uint32), total-sizeof(uint32));
        else
            name_size = Socket_Read(rfd, pack+sizeof(uint32), total-sizeof(uint32));

        /*  if(slot->backend_fd != 0){
            if(q !=0 || ask == 'Q'){ 
                Socket_Send(wfd, &ask, sizeof(char));
                Socket_Send(wfd, pack, psize);
                //free(pack);
            }
        }else{*/
        if(type == 1){
            Socket_Send(wfd, &ask, sizeof(char));
            Socket_Send(wfd, pack, psize);
        }else{
            Socket_Send(wfd, &ask, sizeof(char));
            Socket_Send(wfd, pack, psize);
        }
        /*  }
       printf("pack:%s\n", pack+sizeof(uint32));
        
        if ( slot->backend_fd == 0 && ask != 'p') {
            MSGFORMAT *_mf;
            _mf = (MSGFORMAT *)calloc(1, sizeof(MSGFORMAT));
            _mf->format = (char *)calloc(1, sizeof(char)+psize);

            memcpy(_mf->format, &ask, sizeof(char));
            memcpy(_mf->format+sizeof(char), pack, psize);                    
            _mf->format_len = sizeof(char)+psize;
           // free(pack);
            if(slot->tail == NULL){
                slot->tail = slot->head = _mf;

            }else{    
                slot->head->next = _mf;

                slot->head = _mf;
            }   
        }*/

        switch ( ask ) {
            case 'R':	

                if(total==8){
                    type = 1;
                }else
                    type = 0;                
                FB(type);
                break;

            case 'p':	
                type = 1;
                FB(type);
                /*  
                if(slot->passwd == NULL){
                    slot->passwd = (char *)calloc(1, sizeof(char));
                    memcpy(slot->passwd, &ask, sizeof(char));
                    slot->passwd = (char *)realloc(slot->passwd, psize);
                    memcpy(slot->passwd, pack+sizeof(char), psize);
                    slot->pw_len = sizeof(char)+psize;
                    //free(pack);
                    FB(1);
                }else{
                    if(_mf == NULL)break;
                    _mf = _mf->next;
                    while ( _mf ){                        
                       // printf("format:%s\n", _mf->format);
                        Socket_Send(ffd, _mf->format, _mf->format_len);
                        _mf = _mf->next;
                    }                    
                    FB(0);
                }*/
                break;            
            case 'S':
                /* if(slot->backend_fd != 0){
                if(_mf){
                    if(_mf->format)
                        printf("_mf:%s\n", _mf->format);
                    _mf = _mf->next;
                    if(_mf) printf("yes _mf\n");
                }
                printf("s\n"); 
                }*/
                type = 1;
                FB(type);
                break;
            
            case 'K':
                type = 1;
                FB(type);
                break;
            case 'C':
                
                break;
            case 'Z':
                //if(slot->backend_fd == 0)
                 //   slot->backend_fd = bfd; 
                type = 0;
                FB(type);
                break;
            
            case 'Q':	
                q = 1;
                type = 1;
                FB(type);
                break;

            case 'T':
                type = 1;	
                FB(type);
                break;
            case 'X':
                return 0;
                
            default:	
                
                break;
        }				/* -----  end switch  ----- */

    }
}

/*
 * pg to client
 */
int PGAuthenticationOk(int fd, char *pack){
    char *newbuf;
    size_t msg_len, psize, plen, res_len;
    uint32 msg, res;

    if(pack == NULL) {
        d_log("pack calloc error !\n");
        return -1;
    }

    psize = sizeof(char);
    plen = Socket_Read(fd, pack, psize);
    if(plen != sizeof(char)) return -1;
        
    if(memcmp(pack, "R", sizeof(char)) != 0) return -1;

    newbuf = realloc(pack, psize);
    if(newbuf){
            pack = newbuf;
    } else return -1;

    msg_len = Socket_Read(fd, pack+sizeof(char), sizeof(uint32));

    if(msg_len != sizeof(uint32)) return -1;

    memcpy(&msg, pack+sizeof(char), sizeof(uint32));

    msg = ntohl(msg);
    psize += msg;
    newbuf = realloc(pack, psize);
    if(newbuf){
            pack = newbuf;
    } else return -1;

    res_len = Socket_Read(fd, pack+sizeof(char)+sizeof(uint32), msg-sizeof(uint32));
    //printf("res_len:%d\n", res_len);
    memcpy(&res, pack+sizeof(char)+sizeof(uint32), msg-sizeof(uint32));
    res = ntohl(res);
     
    return psize;

}
/* 
 * client to pg
 * */
int PGPasswordMessage(int fd, char *pack){
    char *newbuf;
    size_t pv_len;
    uint32 len;

    if(pack == NULL) {
        d_log("pack calloc error !\n");
        return -1;
    }

    pv_len = Socket_Read(fd, pack, sizeof(char));
    if(pv_len != sizeof(char)) return -1;
    
    if(memcmp(pack, "p", sizeof(char)) != 0)return -1;

    pv_len = Socket_Read(fd, pack+sizeof(char), sizeof(uint32));
    if(pv_len != sizeof(uint32)) return -1;

   
    memcpy(&len, pack+sizeof(char), sizeof(uint32));

    len = ntohl(len);

    newbuf = realloc(pack, len+sizeof(char));
    if(newbuf){
        pack = newbuf;
    }else return -1;

    pv_len = Socket_Read(fd, pack+sizeof(char)+sizeof(uint32), len-sizeof(uint32));
    //printf("passwd:%s\n", pack+sizeof(char)+sizeof(uint32));
    if(pv_len == (len-sizeof(uint32))){
        return sizeof(char)+len;
    }
    
    return -1;

}

/*
 * pg to client
 */
int PGAuthenticationMD5Password(int fd, char *pack){
    char *newbuf ,*encry;
    size_t psize, cmd_size, total_size, auth_size, encry_size;
    uint32 total, auth;

    if(pack == NULL) {
        d_log("pack calloc error !\n");
        return -1;
    }
    
    psize = sizeof(char);
    cmd_size = Socket_Read(fd, pack, psize);
    if(cmd_size != sizeof(char)) return -1;
        
    if(memcmp(pack, "R", sizeof(char)) != 0) return -1;
    
    newbuf = realloc(pack, psize+sizeof(uint32));
    if(newbuf){
            pack = newbuf;
    } else return -1;

    total_size = Socket_Read(fd, pack+psize, sizeof(uint32));

    if(total_size != sizeof(uint32)) return -1;

    memcpy(&total, pack+sizeof(char), sizeof(uint32));

    total = ntohl(total);

    psize += total;
    newbuf = realloc(pack, psize);
    if(newbuf){
            pack = newbuf;
    } else return -1;

    auth_size = Socket_Read(fd, pack+sizeof(char)+sizeof(uint32), sizeof(uint32));
    //printf("res_len:%d\n", res_len);
     memcpy(&auth, pack+sizeof(char)+sizeof(uint32), sizeof(uint32));
     auth = ntohl(auth);
     //printf("auth:%d\n", auth);
   
     
    encry_size = Socket_Read(fd, pack+sizeof(char)+sizeof(uint32)+sizeof(uint32), total-sizeof(uint32)-sizeof(uint32));
    /*printf("encry_size:%d\n", encry_size);
    memcpy(&encry, pack+sizeof(char)+sizeof(uint32)+sizeof(uint32), total-sizeof(uint32)-sizeof(uint32));
    printf("encry:%s\n", pack+sizeof(char)+sizeof(uint32)+sizeof(uint32));*/

     
    return psize;
}

int PGParameterStatus(int fd, char *pack){
    char *newbuf, *p;
    size_t psize, cmd_size, total_size, name_size, encry_size;
    uint32 total, encry;

    if(pack == NULL) {
        d_log("pack calloc error !\n");
        return -1;
    }
    
    psize = sizeof(char);
    cmd_size = Socket_Read(fd, pack, psize);
    if(cmd_size != sizeof(char)) return -1;
        
    if(memcmp(pack, "S", sizeof(char)) != 0) return -1;
    
    newbuf = realloc(pack, psize+sizeof(uint32));
    if(newbuf){
            pack = newbuf;
    } else return -1;

    total_size = Socket_Read(fd, pack+psize, sizeof(uint32));

    if(total_size != sizeof(uint32)) return -1;
    memcpy(&total, pack+sizeof(char), sizeof(uint32));

    total = ntohl(total);

    psize += total;
    newbuf = realloc(pack, psize);
    if(newbuf){
            pack = newbuf;
    } else return -1;

    name_size = Socket_Read(fd, pack+sizeof(char)+sizeof(uint32), total-sizeof(uint32));

    return psize;
}

/*
 * 
 */
int PGBackendKeyData(int fd, char *pack){
    char *newbuf;
    size_t psize, cmd_size, total_size, name_size, encry_size;
    uint32 total, encry;

    if(pack == NULL) {
        d_log("pack calloc error !\n");
        return -1;
    }
    
    psize = sizeof(char);
    /* cmd_size = Socket_Read(fd, pack, psize);
    if(cmd_size != sizeof(char)) return -1;
       */ 
    if(memcmp(pack, "K", sizeof(char)) != 0) return -1;
    
    newbuf = realloc(pack, psize+sizeof(uint32));
    if(newbuf){
            pack = newbuf;
    } else return -1;

    total_size = Socket_Read(fd, pack+psize, sizeof(uint32));

    if(total_size != sizeof(uint32)) return -1;
    memcpy(&total, pack+sizeof(char), sizeof(uint32));

    total = ntohl(total);

    psize += total;
    newbuf = realloc(pack, psize);
    if(newbuf){
            pack = newbuf;
    } else return -1;
    name_size = Socket_Read(fd, pack+sizeof(char)+sizeof(uint32), total-sizeof(uint32));

    return psize;
}

/* 
 *  */

int PGReadyForQuery(int fd, char *pack){
    char *newbuf;
    size_t psize, cmd_size, total_size, name_size, encry_size;
    uint32 total, encry;

    if(pack == NULL) {
        d_log("pack calloc error !\n");
        return -1;
    }
    
    psize = sizeof(char);
    cmd_size = Socket_Read(fd, pack, psize);
    if(cmd_size != sizeof(char)) return -1;
        
    if(memcmp(pack, "Z", sizeof(char)) != 0) return -1;
    
    newbuf = realloc(pack, psize+sizeof(uint32));
    if(newbuf){
            pack = newbuf;
    } else return -1;

    total_size = Socket_Read(fd, pack+psize, sizeof(uint32));

    if(total_size != sizeof(uint32)) return -1;
    memcpy(&total, pack+sizeof(char), sizeof(uint32));

    total = ntohl(total);

    psize += total;
    newbuf = realloc(pack, psize);
    if(newbuf){
            pack = newbuf;
    } else return -1;

    name_size = Socket_Read(fd, pack+sizeof(char)+sizeof(uint32), total-sizeof(uint32));

    return psize;
}

int PGSimpleQuery(int fd, char *pack){
    char *newbuf;
    size_t psize, cmd_size, total_size, name_size, encry_size;
    uint32 total, encry;

    if(pack == NULL) {
        d_log("pack calloc error !\n");
        return -1;
    }
    
    psize = sizeof(char);
    cmd_size = Socket_Read(fd, pack, psize);
    if(cmd_size != sizeof(char)) return -1;
        
    if(memcmp(pack, "Q", sizeof(char)) != 0) return -1;
    
    newbuf = realloc(pack, psize+sizeof(uint32));
    if(newbuf){
            pack = newbuf;
    } else return -1;

    total_size = Socket_Read(fd, pack+psize, sizeof(uint32));

    if(total_size != sizeof(uint32)) return -1;
    memcpy(&total, pack+sizeof(char), sizeof(uint32));

    total = ntohl(total);

    psize += total;
    newbuf = realloc(pack, psize);
    if(newbuf){
            pack = newbuf;
    } else return -1;

    name_size = Socket_Read(fd, pack+sizeof(char)+sizeof(uint32), total-sizeof(uint32));

    return psize;

}
/* vim: set ts=4 sw=4: */

