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

/* select  start */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hfind
 *  Description: 
 * =====================================================================================
 */
HITEM *hfind ( ub1 *key, ub4 keyl ){
    ub4 hval,hjval, y;
    HITEM *ph;
    TLIST *tlist;
    int i;

    hval = lookup(key, keyl, 0);
    hjval = jenkins_one_at_a_time_hash(key, keyl);

    tlist = pools_tlist->next;
     
    if(!pools_hitem){
        perror("shmat pools_hitem");
        return NULL;
    }
    ph = pools_hitem[(hval&pools_htab->mask)];
    if(!ph) return NULL;
     
    while ( ph ) {
        if(hval == ph->hval &&
            (keyl == ph->keyl) &&
            (hjval == ph->hjval) 
            ){
                
                while ( tlist ) {
                    /*  has a bug */
                    if(strstr(key, tlist->name)){
                        /* over time */
                        if(tlist->utime > ph->utime) return NULL; 
                    }
                    tlist = tlist->next;
                }

                HIT_LOCK();
                ph->ahit++;
                pools_htab->hit++;
                for(i=0; i<MAX_HARU_POOL; i++){
                    if(pools_haru_pool[i].hval == hval){
                        pools_haru_pool[i].hit++;
                        break;
                    }
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
char *getslab ( HITEM *hitem){
    HITEM *_ph = hitem;
    HSLAB *_ps;
    int i;
    char *res;

    i = hsms(_ph->psize);
    _ps = pools_hslab[i];

    for(; _ps; _ps=ps->next){
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
int addHdr ( HDR *myhdr; int m ){
    HDR *_hdr, *_t;

    _hdr = pools_hdr[m];
    if(_hdr->pid != myhdr->pid) return -1;
    if(myhdr->skl > LIMIT_SLAB_BYTE) return -1;

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
int addulist ( ULIST *mlist, int m ){
    ULIST *_ulist, *_t;
    
    _ulist = pools_ulist[m];
    if(_ulist->pid != mlist->pid) return -1; 

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

