/*
 * =====================================================================================
 *
 *       Filename:  pool_proc.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/11/2013 03:25:19 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include "pool_proc.h"

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hproc
 *  Description:  
 * =====================================================================================
 */
void hproc ( ){

    /* update table tlist 
    htlist();*/

    /* hdr to hitem  */
    fetchdti(); 
   
}		/* -----  end of function hproc  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  fetchdti
 *  Description:  fetch pools_hdr to pools_hitem , use haru 
 * =====================================================================================
 */
void fetchdti (  ){
    HDR *pd, *_pd;

    pd = pools_hdr_tail;
    
    while(pd){

        if(pd->flag == H_FALSE){ 
            goto clear;
        }
        if(!pd->key){
            goto clear;
        }
        haddHitem(pd);

        clear:
            if(pd == pools_hdr_head){
                pd->flag = H_FALSE;
                pools_hdr_tail = pd;
                break;
            }else{
                _pd = pd;
                pd = pd->next;
                freeHdr(_pd);                
            }
    }   
}		/* -----  end of function fetchdti  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  haddHitem
 *  Description:  hdr to hitem
 * =====================================================================================
 */
word haddHitem ( HDR *mhdr ){ 
    HITEM  *ph, *hp;
    HDR *hdr;
    ub4    x, y ;
    uint64_t _new_hval, _new_hjval;
    int i, m, n, slab_id;
    ub1 *slab_sm;
    HG *pool_hg;
    HROW *_hrow;

    hdr = mhdr;
    if(hdr == NULL) return -1;
    if(hdr->drl > LIMIT_SLAB_BYTE) return -1;
    
    _new_hval = lookup(hdr->key, hdr->keyl,0);
    _new_hjval = jenkins_one_at_a_time_hash(hdr->key, hdr->keyl);

    n = 0;
    i = hsms(hdr->drl);
    if(i == -1) return -1;

    do{
        pool_hg = hitem_group[n];
        if(pool_hg == NULL) break;
        x=(_new_hval&pool_hg->mask);
        _hrow = pool_hg->hrow + x;
        y = _new_hjval&(MAX_HITEM_LENGTH-1);
        ph = _hrow->hitem + y;

        if(!ph) {
            DEBUG("ph is null");
            break;
        }
        if(ph->hval == 0 || ph->drl == 0){
            hp = ph;
            if(hp->keyl > KEY_LENGTH){
               free(hp);
               DEBUG("keyl > KEY_LENGTH");
               return -1;
            }
             
            if(pools_fslab[i].sa != 0){
                hp->sid = pools_fslab[i].sid;
                hp->sa = pools_fslab[i].sa;
                pools_fslab[i].sa = 0;
            }else{
                slab_id = findslab(slabclass[i].size);
                if(slab_id == -1){
                    DEBUG("slab_id == -1 psize: %d", hp->psize);
                    return -1;
                }
                hp->sid = slab_id;
                hp->sa = pools_hslab[slab_id].ss - slabclass[i].size;
            }

            memcpy(hp->key, hdr->key, hdr->keyl);
            hp->keyl  = hdr->keyl;
            hp->drl = hdr->drl;
            hp->psize = slabclass[i].size;
            hp->hval  = _new_hval;
            hp->hjval = _new_hjval;
            hp->utime = hdr->stime;
            slab_sm = pools_hslab[hp->sid].sm + hp->sa;

            if(hp->sa < 0){ 
                DEBUG("psize error! sa: %d psize: %d", hp->sa, hp->psize);
            }else{
                memcpy(slab_sm , hdr->dr, hdr->drl);
                
                //pools_hitem_row[i]++;

                //hrule(hp, H_INSERT); 
            }
            pools_htab->count++; 
            pools_htab->miss++;
            pool_hg->count++;
            return 0;     
        }
        else if(_new_hval == ph->hval &&
            (hdr->keyl == ph->keyl) &&
            (_new_hjval == ph->hjval) 
            ){
            m = hsms(ph->psize);
            if(m == -1) return -1; 
            if(m != i){  /* old size != new size , free old slab */
                addfslab(ph);  /* free old slab */
                if(pools_fslab[i].sa != 0){
                    ph->sid = pools_fslab[i].sid;
                    ph->sa = pools_fslab[i].sa;
                    pools_fslab[i].sa = 0;
                }else{
                    slab_id = findslab(slabclass[i].size);
                    if(slab_id == -1) return -1;
                    ph->sid = slab_id;
                    ph->sa = pools_hslab[slab_id].ss - slabclass[i].size;
                }
                ph->psize = slabclass[i].size;  /* update psize */                
            }
            /* 
            if((ph->amiss / ph->ahit) > LIMIT_PERCENT){
                ph->drl = 0;
                break;
            }*/
             
            if(ph->sa < 0)return -1;
            slab_sm = pools_hslab[ph->sid].sm + ph->sa;
            if(!slab_sm) return -1;
            ph->utime = hdr->stime;
            memcpy(slab_sm, hdr->dr, hdr->drl);
            ph->drl = hdr->drl;
            ph->amiss++;
            pools_htab->miss++;
            //hrule(ph, H_UPDATE);
            return 0;
        }else{
            DEBUG("x:%d, y:%d", x, y);                  
            n++;
            hgrow(n);
        }
        
    }while(n<MAX_HG_LENGTH);

    return -1;
}		/* -----  end of function haddHitem  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hrule
 *  Description:  
 * =====================================================================================
 */
void hrule ( HITEM *hitem, H_CHANGE hstat ){
    int i;
    HITEM *ph;

    if(!hitem) return;
    /*
    if(pools_htab->bytes == conn_global->maxbytes &&
        pools_harug->step == MAX_HARU_POOL){
        for ( i=0; i<MAX_HARU_POOL; i++ ) {
            ph = pools_haru_pool[i].phitem;
            addfslab(ph);  / free old slab /
        }   
        pools_harug->step = 0;
    }

    for(i=0; i<pools_harug->step; i++){
        ph = pools_haru_pool[i].phitem;
        if(hitem->hval == ph->hval &&
            (hitem->keyl == ph->keyl) &&
            (hitem->hjval == ph->hjval) 
            ){
            return;
        } 
    }

    if(pools_harug->step < MAX_HARU_POOL){
        pools_haru_pool[pools_harug->step].hit = 0;
        pools_haru_pool[pools_harug->step++].phitem = hitem;
        return;
    } else{

        / MRU /    
        
        if( hstat == H_UPDATE ){
            pools_haru_pool[pools_harug->max].hit = 0;
            pools_haru_pool[pools_harug->max].phitem = hitem;
        }
        else{
            / LRU /
            ph = pools_haru_pool[pools_harug->mix].phitem;
            pools_haru_pool[pools_harug->mix].phitem = hitem;
            pools_haru_pool[pools_harug->mix].hit = 0;

            addfslab(ph);   clear hit very limit /
        }
    } 
  */
}		/* -----  end of function hrule  ----- */

 /* vim: set ts=4 sw=4: */

