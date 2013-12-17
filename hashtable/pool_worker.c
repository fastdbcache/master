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
 *         Name:  hkey
 *  Description:  
 * =====================================================================================
 */
char *hkey ( char *key, ub4 keyl){
    HITEM *_h;

    _h = hfind(key, keyl);

    return getslab(_h);
}		/* -----  end of function hkey  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hfind
 *  Description: 
 * =====================================================================================
 */
HITEM *hfind ( char *key, ub4 keyl ){
    ub4 hval,hjval, y;
    HITEM *ph;
    TLIST *tlist;
    int i;
    
    hval = lookup((ub1 *)key, keyl, 0);
    hjval = jenkins_one_at_a_time_hash((ub1 *)key, keyl);

    tlist = pools_tlist->next;

    HITEM_SWITCH((y=(hval&pools_htab->mask)));
    ph = pools_hitem[y];
     
    if(!ph) return NULL;
     
    while ( ph ) {
        if(hval == ph->hval &&
            (keyl == ph->keyl) &&
            (hjval == ph->hjval) &&
            (ph->drl > 0) 
            ){
                
                while ( tlist ) {
                    /*  has a bug */
                    if(strstr(key, tlist->key)){
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
char *getslab ( HITEM * hitem){
    HITEM *_ph = hitem;
    HSLAB *_ps;
    int i;
    char *res;

    if(!_ph) return NULL;

    i = hsms(_ph->psize);
    _ps = pools_hslab[i];

    for(; _ps; _ps=_ps->next){
        if(_ps->id == _ph->sid){
            res = calloc(_ph->drl, sizeof(char));
            if(!res) return NULL;
            memcpy(res, _ps->sm+_ph->sa*_ph->psize, _ph->drl);
            return res;
        }
    }
    return NULL;
}		/* -----  end of function getslab  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  addHdr
 *  Description:  
 * =====================================================================================
 */
int addHdr ( HDR *myhdr, int m ){
    HDR *_hdr, *_t;

    _hdr = pools_hdr[m];
    /* if(_hdr->pid != myhdr->pid) return -1;*/
    if(myhdr->drl > LIMIT_SLAB_BYTE) return -1;

    /* clear nasty hdr */
    for(;_hdr->next;_hdr=_hdr->next){
        _t = _hdr->next;
        if(_t->flag == H_FALSE){
            _hdr->next = _t->next;
            freeHdr(_t);
            if(!_hdr->next) break;
        }
    }
        
    for (; _hdr->next; _hdr=_hdr->next );;
    _hdr->next = myhdr;

    return 0;
}		/* -----  end of function addHdr  ----- */

/* select  end */

/* change start */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  addulist
 *  Description:  
 * =====================================================================================
 */
int addUlist ( ULIST *mlist, int m ){
    ULIST *_ulist, *_t;
    
    _ulist = pools_ulist[m];
    /*if(_ulist->pid != mlist->pid) return -1; */

    /* clear nasty ulist */
    for(; _ulist->next; _ulist=_ulist->next){
        _t = _ulist->next;
        if(_t->flag == H_FALSE){            
            _ulist->next = _t->next;
            freeUList(_t); 

            if(!_ulist->next) break;
        }
    }

    for(;_ulist->next;_ulist=_ulist->next);;
    _ulist->next = mlist;
    
    return 0;
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

    free(fhdr->key);
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

    free(flist->key);
    free(flist);
}		/* -----  end of function freeUList  ----- */
 /* vim: set ts=4 sw=4: */

