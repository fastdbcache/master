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

    i = 0;
    if(!key){DEBUG("key error %d",i); return NULL;}

    hval = lookup((ub1 *)key, keyl, 0);
    hjval = jenkins_one_at_a_time_hash((ub1 *)key, keyl);

    tlist = pools_tlist->next;

    HITEM_SWITCH((y=(hval&pools_htab->mask)));
    ph = pools_hitem[y]->next;
    
     
    while ( ph ) {
        DEBUG("hval:%lu hjval:%lu", hval, hjval);
        DEBUG("hval:%lu hjval:%lu, keyl:%lu", ph->hval, ph->hjval, ph->keyl);
        if(hval == ph->hval &&
            (keyl == ph->keyl) &&
            (hjval == ph->hjval) &&
            (ph->drl > 0) 
            ){
                DEBUG("start--\nkey:%s", key);
                
                while ( tlist ) {
                    /*  has a bug */
                    if(tlist->keyl >0 && strstr(key, tlist->key)){
                        /* over time */
                        if(tlist->utime > ph->utime) return NULL; 
                    }
                    tlist = tlist->next;
                }
                DEBUG("ph key:%s",ph->key);
                DEBUG("end %d",ph->sid);
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
            dest->pack = calloc(_ph->drl, sizeof(char));
            if(!dest->pack) return;
            memcpy(dest->pack, _ps->sm+_ph->sa*_ph->psize, _ph->drl);
            /*  dest->pack = _ps->sm+_ph->sa*_ph->psize;*/
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

