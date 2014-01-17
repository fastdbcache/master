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
    HITEM  *ph,*phtmp, *hp;
    HDR *hdr;
    ub4     y ;
    uint64_t _new_hval, _new_hjval;
    HSLAB  *hsp;
    FSLAB *fslab;
    HSLAB *hslab;
    int i, m;
    HITEM **pools_hitem;
    ub1 md5[MD5_LENG];
    MD5_CTX *ctx;

    hdr = mhdr;
    if(hdr == NULL) return -1;
    if(hdr->drl > LIMIT_SLAB_BYTE) return -1;

    /*bzero(md5, MD5_LENG);
    ctx = calloc(1, sizeof(MD5_CTX));
    MD5_Init(ctx);
    MD5_Update(ctx, hdr->key, hdr->keyl);
    MD5_Final(md5, ctx);
    free(ctx);
    */
    _new_hval = lookup(hdr->key, hdr->keyl,0);
    _new_hjval = jenkins_one_at_a_time_hash(hdr->key, hdr->keyl);
    HITEM_SWITCH((y=(_new_hval&pools_htab->mask)));

    ph = pools_hitem[y];
    if(!ph) {
        return -1;
    }
    i = hsms(hdr->drl);
    if(i == -1) return -1;

    /* header is not user 
    for(; ph->next; ph=ph->next){
        if(_new_hval == ph->hval &&
            (hdr->keyl == ph->keyl) &&
            (_new_hjval == ph->hjval) 
            ){
            MISS_LOCK();
            pools_htab->miss++;
            MISS_UNLOCK(); 
            break;
        }
    }*/
    m = 0;
    phtmp = ph;
    ph = ph->next;  /* head is not store anything */
    while ( ph ){
        phtmp = ph; 
        if ((_new_hval == ph->hval) && 
            (hdr->keyl == ph->keyl) && 
            (_new_hjval == ph->hjval))
        {
            m = hsms(ph->psize);
            if(m == -1) return -1; 
            if(m != i){  /* old size != new size , free old slab */
                addfslab(ph);  /* free old slab */

                fslab = findslab(slabclass[i].size, i);
                if(!fslab) return -1;
                ph->psize = slabclass[i].size;  /* update psize */
                ph->sid = fslab->sid;
                ph->sa = fslab->sa;
                ph->drl = hdr->drl;
                
                free(fslab);
            }
            /* 
            if((ph->amiss / ph->ahit) > LIMIT_PERCENT){
                ph->drl = 0;
                break;
            }*/
            
            hsp = findhslab(i, ph->sid);    
            if(hsp != NULL){
                ph->utime = hdr->stime;
                memcpy(hsp->sm+ph->sa*ph->psize, hdr->dr, hdr->drl);
                ph->drl = hdr->drl;
                ph->amiss++;
                pools_htab->miss++;
                hrule(ph, H_UPDATE);
                return 0;
            }else{
                DEBUG("hsp error");
                return 0;
            }
            break;            
        }
        ph = ph->next; 
    } /* while */
    
    if(!phtmp->next){
        hp = hitemcreate();
        hp->key   = (ub1 *)calloc(hdr->keyl, sizeof(ub1));
        memcpy(hp->key, hdr->key, hdr->keyl);
        hp->keyl  = hdr->keyl;
        hp->drl = hdr->drl;
        hp->psize = slabclass[i].size;
        hp->hval  = _new_hval;
        hp->hjval = _new_hjval;
        hp->utime = hdr->stime; 
        fslab = findslab(hp->psize, i);
        hslab = findhslab(i, fslab->sid); 
        
        hp->sid = fslab->sid;
        hp->sa = fslab->sa;
        free(fslab);

        if(hp->sa*hp->psize > LIMIT_SLAB_BYTE){ 
            DEBUG("psize error! sa: %d psize: %d", hp->sa, hp->psize);
        }else{
            memcpy(hslab->sm+hp->sa*hp->psize, hdr->dr, hdr->drl);
            
            phtmp->next = hp;

            pools_hitem_row[i]++;

            hrule(hp, H_INSERT); 
        }
    }
 
    /* make the hash table bigger if it is getting full */
    if (++pools_htab->count > pools_htab->logsize)
    {
        DEBUG("now expand hashtable count %d logsize %d", pools_htab->count, pools_htab->logsize);
        hgrow();
    }
    return TRUE;
}		/* -----  end of function haddHitem  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hrule
 *  Description:  
 * =====================================================================================
 */
void hrule ( HITEM *hitem, H_CHANGE stat ){
    int i;
    HITEM *ph;

    if(!hitem) return;

    if(pools_htab->bytes == conn_global->maxbytes &&
        pools_harug->step == MAX_HARU_POOL){
        for ( i=0; i<MAX_HARU_POOL; i++ ) {
            ph = pools_haru_pool[i].phitem;
            addfslab(ph);  /* free old slab */
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

        /* MRU */    
        
        if( stat == H_UPDATE ){
            pools_haru_pool[pools_harug->max].hit = 0;
            pools_haru_pool[pools_harug->max].phitem = hitem;
        }
        else{
            /* LRU */
            ph = pools_haru_pool[pools_harug->mix].phitem;
            pools_haru_pool[pools_harug->mix].phitem = hitem;
            pools_haru_pool[pools_harug->mix].hit = 0;

            addfslab(ph);  /* clear hit very limit */
        }
    } 
}		/* -----  end of function hrule  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hsort
 *  Description:  
 * =====================================================================================
 */
int hsort (  ){
   /*  int i, m;
    HARU  *t;

    t = pools_harug->haru_pool[0];
    m = 0;
    for ( i=1; i<MAX_HARU_POOL-1; i++ ) {
        if(t->hit < pools_harug->haru_pool[i].hit) {
            t = pools_harug->haru_pool[i];
            m = i;
        }
    }

    return m;*/
    return 0;
}		/* -----  end of function hsort  ----- */

 /* vim: set ts=4 sw=4: */

