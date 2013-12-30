/*
 * =====================================================================================
 *
 *       Filename:  pool_worker.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/11/2013 11:17:12 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "pool_worker.h"
/* select  start */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  listHslab
 *  Description:  
 * =====================================================================================
 */
void listHslab (  ){
    HITEM *_h;
    int i, m;
    HITEM **pools_hitem;

    for(i=0; i<pools_htab->logsize; i++){
        HITEM_SWITCH(i);
        _h = pools_hitem[i];
        if(_h->next) _h = _h->next;
        else continue;
        for(; _h; _h=_h->next){
            m=0;
            printf("%d.hitem:%d\n",m++, i);
            printf("%d.hitem:key:%s\n",m++,_h->key);
            printf("%d.hitem:keyl:%lu\n",m++,_h->keyl);
            printf("%d.hitem:drl:%lu\n",m++,_h->drl);
            printf("%d.hitem:psize:%lu\n",m++,_h->psize);
            printf("%d.hitem:sid:%d\n",m++,_h->sid);
            printf("%d.hitem:sa:%lu\n",m++,_h->sa);
            printf("%d.hitem:hval:%lu\n",m++,_h->hval);
            printf("%d.hitem:hjval:%lu, %lu\n",m++,_h->hjval, jenkins_one_at_a_time_hash(_h->key, _h->keyl));
            printf("%d.hitem:utime:%lu\n",m++,_h->utime);
            printf("%d.hitem:ahit:%lu\n",m++,_h->ahit);
            printf("%d.hitem:amiss:%lu\n",m++,_h->amiss);
            printf("%d.end %d\n",m++, i);
        }
    }
   /*
    for(i=0; slabclass[i].chunk; i++){
        DEBUG("chunk %d, size:%d", slabclass[i].chunk, slabclass[i].size);
        _h = pools_hslab[i];
        if(_h && _h->sm){
            for(;_h;_h=_h->next){
                DEBUG();
            }
        }
    }  */ 
}		/* -----  end of function listHslab  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  setCacheRowDescriptions
 *  Description:  
 * =====================================================================================
 */
void setCacheRowDescriptions ( int frontend ){
    char *crd, *field, *newbuf;
    uint32 total, len;
    int i, nfields;
    int         tableid;
    int         columnid;
    int         typid;
    int         typlen;
    int         atttypmod;
    int         format;
    char *rowdes[]={"key","keyl","drl","psize","sid","sa","hval","hjval","utime","ahit","amiss"};
    char res[32]={'\0'};
    total = 0;
    nfields = 11;

    total += sizeof(uint32)+sizeof(uint16);

    field = NULL;
    for(i=0; i<nfields; i++){
        total += strlen(rowdes[i])+1+sizeof(uint32)+sizeof(uint16)+sizeof(uint32)+sizeof(uint16)+sizeof(uint32)+sizeof(uint16);
    }

    crd = calloc(total+sizeof(char), sizeof(char));
    newbuf = crd;
    memcpy(crd, "T", sizeof(char));
    crd+=sizeof(char);
    len = total; 
    total = htonl(total);
    memcpy(crd, &total, sizeof(uint32));
    crd+=sizeof(uint32);

    nfields = htons(nfields);    
    memcpy(crd, &nfields, sizeof(uint16));
    crd+=sizeof(uint16);

    
    for(i=0; i<11; i++){
        
        memcpy(crd,rowdes[i],strlen(rowdes[i]));
        crd += strlen(rowdes[i])+1;
        tableid = htonl(0); 
        memcpy(crd, &tableid, sizeof(uint32));
        crd+=sizeof(uint32);
        columnid = htons(i+1);
        memcpy(crd,&columnid , sizeof(uint16));
        crd+=sizeof(uint16);
        typid = htonl(25);
        memcpy(crd,&typid , sizeof(uint32));
        crd+=sizeof(uint32);
        typlen = htons(65535);
        memcpy(crd, &typlen, sizeof(uint16));
        crd+=sizeof(uint16);
        atttypmod = htonl(-1);
        memcpy(crd,&atttypmod , sizeof(uint32));
        crd+=sizeof(uint32);
        format = htons(0);
        memcpy(crd,&format , sizeof(uint16));
        if(i!=(nfields-1))
            crd+=sizeof(uint16);
    }
    Socket_Send(frontend, newbuf, len+sizeof(char));

    free(newbuf);
    
    HITEM *_h;
    int  m, nf, tlen, _tlen;
    uint32 _ulen;
    HITEM **pools_hitem;
    char *tmp, *_tmp, *_newtmp;
    _tlen = 0; 
    tmp = NULL;
    m=0;
    for(i=0; i<pools_htab->logsize; i++){
        HITEM_SWITCH(i);
        _h = pools_hitem[i];
        if(_h->next) _h = _h->next;
        else continue;
        for(; _h; _h=_h->next){
            if(_tlen==0){
                _tlen = _h->keyl+sizeof(_h->keyl)+sizeof(_h->drl)+sizeof(_h->psize)
                    + sizeof(_h->sid)+sizeof(_h->sa)+sizeof(_h->hval)+sizeof(_h->hjval)
                    +sizeof(_h->utime)+sizeof(_h->ahit)+sizeof(_h->amiss)+(11*sizeof(uint32));
                tlen = _tlen;
            }
            m++;
            if(tmp == NULL){
                tmp = calloc(tlen, sizeof(char));
                _newtmp = tmp;
            }else{
                _tmp = realloc(tmp, (tlen+_tlen));
                if(_tmp){
                    tmp = _tmp;
                    _newtmp = tmp;
                    tmp += tlen;
                    tlen+=_tlen;
                }return ;
            }
            _ulen = htonl(_h->keyl);
            memcpy(tmp, &_ulen, sizeof(uint32));
            tmp+=sizeof(uint32);
            memcpy(tmp, _h->key, _h->keyl);
            tmp += _h->keyl;

            _ulen = htonl(sizeof(_h->keyl));
            memcpy(tmp, &_ulen, sizeof(uint32));
            tmp+=sizeof(uint32);
            memcpy(tmp, &_h->keyl, sizeof(_h->keyl));
            tmp += sizeof(_h->keyl);

            _ulen = htonl(sizeof(_h->drl));
            memcpy(tmp, &_ulen, sizeof(uint32));
            tmp+=sizeof(uint32);
            memcpy(tmp, &_h->drl, sizeof(_h->drl));
            tmp += sizeof(_h->drl);

            _ulen = htonl(sizeof(_h->psize));
            memcpy(tmp, &_ulen, sizeof(uint32));
            tmp+=sizeof(uint32);
            memcpy(tmp, &_h->psize, sizeof(_h->psize));
            tmp += sizeof(_h->psize);

            _ulen = htonl(sizeof(_h->sid));
            memcpy(tmp, &_ulen, sizeof(uint32));
            tmp+=sizeof(uint32);
            memcpy(tmp,&_h->sid, sizeof(_h->sid));
            tmp += sizeof(_h->sid);

            _ulen = htonl(sizeof(_h->sa));
            memcpy(tmp, &_ulen, sizeof(uint32));
            tmp+=sizeof(uint32);
            memcpy(tmp, &_h->sa, sizeof(_h->sa));
            tmp += sizeof(_h->sa);

            _ulen = htonl(sizeof(_h->hval));
            memcpy(tmp, &_ulen, sizeof(uint32));
            tmp+=sizeof(uint32);
            memcpy(tmp, &_h->hval, sizeof(_h->hval));
            tmp += sizeof(_h->hval);

            _ulen = htonl(sizeof(_h->hjval));
            memcpy(tmp, &_ulen, sizeof(uint32));
            tmp+=sizeof(uint32);
            memcpy(tmp,&_h->hjval, sizeof(_h->hjval));
            tmp += sizeof(_h->hjval);

            _ulen = htonl(sizeof(_h->utime));
            memcpy(tmp, &_ulen, sizeof(uint32));
            tmp+=sizeof(uint32);
            memcpy(tmp, &_h->utime, sizeof(_h->utime));
            tmp += sizeof(_h->utime);

            _ulen = htonl(sizeof(_h->ahit));
            memcpy(tmp, &_ulen, sizeof(uint32));
            tmp+=sizeof(uint32);
            memcpy(tmp, &_h->ahit, sizeof(_h->ahit));
            tmp += sizeof(_h->ahit);
        
            _ulen = htonl(sizeof(_h->amiss));
            memcpy(tmp, &_ulen, sizeof(uint32));
            tmp+=sizeof(uint32);
            memcpy(tmp, &_h->amiss, sizeof(_h->amiss));
        }
    }
    if(m>0){
        total = sizeof(uint32)+sizeof(uint16) + tlen;
        crd = calloc(total+sizeof(char), sizeof(char));
        newbuf = crd;
        memcpy(crd, "D", sizeof(char));
        crd+=sizeof(char);
        len = total;
        total = htonl(total);
        memcpy(crd, &total, sizeof(uint32));
        crd += sizeof(uint32);
        DEBUG("m %d 11: %d", m, m*11);
        nf = htons((m*11));
        memcpy(crd, &nf, sizeof(uint16));
        crd += sizeof(uint16);
        memcpy(crd, &_newtmp, tlen);

        char *ds, data[1024];
        uint16 num;
        uint32 nlen;
        num = ntohs(nf);
        ds = _newtmp;
        for(;num>0;num--){
            memcpy(&nlen, ds, sizeof(32));
            nlen = ntohl(nlen);
            DEBUG("nlen %d", nlen);
            ds+=sizeof(32);
            bzero(data, 1024);
            memcpy(data, ds, nlen);
            DEBUG("len:%d, byte:%s",nlen, data );
            ds+=len;
        } 

        Socket_Send(frontend, newbuf, len+sizeof(char));
        free(newbuf);
        free(_newtmp);
    }
    DEBUG("C---Z tlen: %d", tlen);
    snprintf(res, 31, "SELECT %d", m);
    total = sizeof(uint32)+strlen(res)+1;
    crd = calloc(total+sizeof(char), sizeof(char));
    newbuf = crd;
    memcpy(crd, "C", sizeof(char));
    crd+=sizeof(char);
    len = total;
    total = htonl(total);
    memcpy(crd, &total, sizeof(uint32));
    crd+=sizeof(uint32);
    memcpy(crd, res, strlen(res)+1);
    Socket_Send(frontend, newbuf, len+sizeof(char));
    free(newbuf);

    
    total = sizeof(uint32)+sizeof(char);
    crd = calloc(sizeof(char)+total, sizeof(char));
    newbuf = crd;
    memcpy(crd, "Z", sizeof(char));
    crd += sizeof(char);
    len = total;
    total = htonl(total);
    memcpy(crd, &total, sizeof(uint32));
    crd+=sizeof(uint32);
    memcpy(crd, "I", sizeof(char)); 
    Socket_Send(frontend, newbuf, len+sizeof(char));
}	/*	 -----  end of function setCacheRowDescriptions  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hkey
 *  Description:  
 * =====================================================================================
 */
void hkey ( char *key, ub4 keyl, SLABPACK *dest){
    HITEM *_h;
    
    _h = hfind(key, keyl);
    
    getslab(_h, dest);
}		/* -----  end of function hkey  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hfind
 *  Description: 
 * =====================================================================================
 */
HITEM *hfind ( char *key, ub4 keyl ){
    ub4  y;
    uint64_t hval, hjval;
    HITEM *ph;
    TLIST *tlist;
    int i;
    HITEM **pools_hitem;
    ub1 md5[MD5_LENG];
    MD5_CTX *ctx;

    i = 0;
    if(!key){DEBUG("key error %d",i); return NULL;}

    bzero(md5, MD5_LENG);
     
    ctx = calloc(1, sizeof(MD5_CTX));
    MD5_Init(ctx);
    MD5_Update(ctx, key, keyl);
    MD5_Final(md5, ctx);
    free(ctx);

    hval = lookup(md5, MD5_LENG, 0);
    hjval = jenkins_one_at_a_time_hash(md5, MD5_LENG);

    tlist = pools_tlist->next;

    HITEM_SWITCH((y=(hval&pools_htab->mask)));
    ph = pools_hitem[y]->next;
    
     
    while ( ph ) {
        if(hval == ph->hval &&
            (keyl == ph->keyl) &&
            (hjval == ph->hjval) &&
            (ph->drl > 0) 
            ){
                
                while ( tlist ) {
                    /*  has a bug */
                    if(tlist->keyl >0 && strstr(key, tlist->key)){
                        /* over time */
                        if(tlist->utime > ph->utime) return NULL; 
                    }
                    tlist = tlist->next;
                }
                HIT_LOCK();
                ph->ahit++;
                pools_htab->hit++;
                for(i=0; i<MAX_HARU_POOL; i++){
                    if( pools_haru_pool[i].phitem == ph ){
                        pools_haru_pool[i].hit++;                        
                        break;
                    }
                }
                if(pools_haru_pool[pools_harug->max].hit < pools_haru_pool[i].hit){
                    pools_harug->max = i;
                }
                if(pools_haru_pool[pools_harug->mix].hit > pools_haru_pool[i].hit){
                    pools_harug->mix = i;
                }
                HIT_UNLOCK();

                return ph;            
        }
        ph = ph->next;
    }
    
    return NULL;
}		/* -----  end of function hfind  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  getslab
 *  Description:  
 * =====================================================================================
 */
void getslab ( HITEM * hitem, SLABPACK *dest){
    HITEM *_ph = hitem;
    HSLAB *_ps;
    int i;

    if(!_ph) return;

    i = hsms(_ph->psize);
    if(i == -1) return ;

    _ps = pools_hslab[i];

    for(; _ps; _ps=_ps->next){
        if(_ps->id == _ph->sid){
            /*dest->pack = calloc(_ph->drl, sizeof(char));
            if(!dest->pack) return;
            memcpy(dest->pack, _ps->sm+_ph->sa*_ph->psize, _ph->drl);*/
              dest->pack = _ps->sm+_ph->sa*_ph->psize;
            dest->len = _ph->drl;
            return;
        }
    }
}		/* -----  end of function getslab  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  addHdr
 *  Description:  
 * =====================================================================================
 */
int addHdr ( HDR *myhdr){

    if(myhdr){
        HDR_LOCK();
        pools_hdr_head->next = myhdr;
        pools_hdr_head = myhdr;
        HDR_UNLOCK();
        return 0;
    }
    
    return -1;
}		/* -----  end of function addHdr  ----- */

/* select  end */

/* change start */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  addulist
 *  Description:  
 * =====================================================================================
 */
int addUlist ( ULIST *mlist){
    
    if(mlist){
        ULIST_LOCK();

        pools_ulist_head->next = mlist;
        pools_ulist_head = mlist;

        ULIST_UNLOCK();
        
        return 0;
    }

    return -1;
}		/* -----  end of function addulist  ----- */

/* change end */
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  freeHdr
 *  Description:  
 * =====================================================================================
 */
void freeHdr ( HDR *fhdr ){
    if(!fhdr)return;
    if(fhdr->key)
        free(fhdr->key);
    if(fhdr->dr)
        free(fhdr->dr);
    free(fhdr);
    fhdr = NULL;
}		/* -----  end of function freeHdr  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  freeUList
 *  Description:  
 * =====================================================================================
 */
void freeUList ( ULIST *flist ){
    if(!flist) return;

    if(flist->key)
        free(flist->key);
    free(flist);
}		/* -----  end of function freeUList  ----- */
 /* vim: set ts=4 sw=4: */

