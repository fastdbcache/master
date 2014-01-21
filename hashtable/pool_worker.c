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
    /*
    bzero(md5, MD5_LENG);
     
    ctx = calloc(1, sizeof(MD5_CTX));
    MD5_Init(ctx);
    MD5_Update(ctx, key, keyl);
    MD5_Final(md5, ctx);
    free(ctx);
    */
    hval = lookup(key, keyl, 0);
    hjval = jenkins_one_at_a_time_hash(key, keyl);

    HITEM_SWITCH((y=(hval&pools_htab->mask)));
    ph = pools_hitem[y]->next;
    
     
    while ( ph ) {
        if(hval == ph->hval &&
            (keyl == ph->keyl) &&
            (hjval == ph->hjval) &&
            (ph->drl > 0) 
            ){
                tlist = pools_tlist->next;
                while ( tlist ) {
                    /*  has a bug */
                    if(keyl >0 && memmem(key, keyl, tlist->key, tlist->keyl)!=NULL){
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
        pushList(mlist->key, mlist->keyl, mlist->utime);
        
        freeUList(mlist);
        return 0;
    }

    return -1;
}		/* -----  end of function addulist  ----- */

/*  
* ===  FUNCTION  ======================================================================
*         Name:  pushList
*  Description:  
* =====================================================================================
*/
void pushList ( char *key, ub4 keyl, ub4 utime ){
    TLIST *_tlist, *_t;

    if(!key) return;
    if(keyl<1) return;

    _tlist = pools_tlist;
    TLIST_LOCK();
    while ( _tlist->next ) {
        _tlist = _tlist->next;
        if( _tlist->keyl == keyl &&
            !memcmp(_tlist->key, key, keyl)
            ){
            if(_tlist->utime < utime){
                _tlist->utime = utime + conn_global->delaytime;  /* delay any time for update */
            }
            break;
        }
    }

    if(!_tlist->next){
        _t = calloc(1, sizeof(TLIST));
        if(_t){
            _t->key = calloc(keyl, sizeof(char));
            if(_t->key){
                DEBUG("table:%s", key);
                memcpy(_t->key, key, keyl);
                _t->keyl = keyl;
                _t->utime = utime + conn_global->delaytime; /* delay any time for update */
                _tlist->next = _t;
                _tlist = _t;
            }
        }
    }
    TLIST_UNLOCK();
}       /*  -----  end of function pushList  ----- */ 

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

