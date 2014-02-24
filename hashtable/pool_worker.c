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
    ub4  y, x;
    uint64_t hval, hjval;
    HITEM *ph;
    TLIST *tlist;
    int i, m;
    HG *pool_hg;
    HROW *_hrow;

    i = m = 0;
    if(!key){DEBUG("key error %d",i); return NULL;}
    if(keyl < 0) return NULL;
    if(keyl > (KEY_LENGTH-1) ) return NULL;

    hval = lookup(key, keyl, 0);
    hjval = jenkins_one_at_a_time_hash(key, keyl);
    
    do{
        pool_hg = hitem_group[m];
        if(pool_hg == NULL){
            /*DEBUG("pool_hg is null m:%d", m);  */
            break;
        }
        x=(hval&pool_hg->mask);
        y = hjval&(MAX_HITEM_LENGTH-1); 
        _hrow = pool_hg->hrow + x;
        ph = _hrow->hitem + y;
        
        if(!ph){
            DEBUG("hp is null");
            break;
        }
        //DEBUG("pool_hg:%llu, _hrow:%llu, ph:%llu,m:%d, key:%s",pool_hg, _hrow, ph, m, key);
        //DEBUG("ph->hval:%llu", ph->drl);
        if(hval == ph->hval &&
            (keyl == ph->keyl) &&
            (hjval == ph->hjval) &&
            (ph->drl != 0) 
            ){
                tlist = pools_tlist->next;
                while ( tlist && tlist->key ) {
                    /*  has a bug */
                    if(memmem(key, (size_t)keyl, tlist->key, (size_t)tlist->keyl)!=NULL){
                        /* over time */
                        if(tlist->utime > ph->utime){
                            /*DEBUG("time out utime %llu, ph utime:%llu", tlist->utime, ph->utime);  */
                            return NULL;
                        } 
                    }
                    tlist = tlist->next;
                }
                HIT_LOCK();
                ph->ahit++;
                pools_htab->hit++;
                                
                HIT_UNLOCK();

                return ph;            
        }
        m++;
    }while(m < pools_htab->gcount);
       
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

    if(!_ph) return;
    if(_ph->sa == -1)return ;
     
    if(pools_hslab[_ph->sid].sm == NULL){
        
        pools_hslab[_ph->sid].sm = hslabcreate(_ph->sid);
        if(pools_hslab[_ph->sid].sm == NULL) return;
    }
    dest->pack = pools_hslab[_ph->sid].sm + _ph->sa + sizeof(uint32)*2;
    dest->len = _ph->drl;
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
    if(!_tlist){
        DEBUG("pools_tlist is null");
        return;
    }
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
            if(keyl < KEY_LENGTH){            
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
    
    free(flist);
}		/* -----  end of function freeUList  ----- */
 /* vim: set ts=4 sw=4: */

