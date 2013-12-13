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


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hproc
 *  Description:  
 * =====================================================================================
 */
void hproc ( ){

    /* update table tlist */
    htlist();

    /* hdr to hitem  */
    fetchdti(); 
   
    return <+return_value+>;
}		/* -----  end of function hproc  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  htlist
 *  Description:  update table list utime 
 * =====================================================================================
 */
void htlist (  ){
    TLIST *_tlist, *_t;
    ULIST *_ulist, *_u;
    char *table;
    _ly *ply;
    int i;
    
    _ulist = pools_utist;

    for(i=0; i<conn_global->process_num; i++){
        _u = _ulist[i];
        _tlist = pools_tlist;
        while(_u->next){
            _u = _u->next;
            if(_u->flag == H_FALSE) continue;
            ply = parser_do (_u->key, _u->keyl);
                        
            while ( _tlist->next ) {
                _tlist = _tlist->next;
                if(!memcmp(_tlist->key,ply->tab, ply->len)&&
                    _tlist->keyl == ply->len){
                    if(_tlist->utime < u->utime){
                        _tlist->utime = u->utime;                        
                        _u->flag = H_FALSE;
                    }        
                    break;
                }
            }
            
            if(!_tlist->next && _u->flag == H_TRUE){
                _t = calloc(1, sizeof(TLIST));
                _t->key = calloc(ply->len, sizeof(char));
                memcpy(_t->key, ply->tab, ply->len);
                _t->keyl = ply->len;
                _u->flag = H_FALSE;  
            }
            free(ply->tab);
            free(ply);
        }
    }

}		/* -----  end of function htlist  ----- */



/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  fetchdti
 *  Description:  fetch pools_hdr to pools_hitem , use haru 
 * =====================================================================================
 */
void fetchdti (  ){
    HITEM *ph, *phtmp;
    HSLAB *ps, *pstmp;
    HARU *pa;
    HDR *pd;
    int i, size, y;
    ub4 hval, hjval;

    pa = pools_haru_pool;    
    
    

    for(i=0; i<conn_global->process_num; i++){
        pd = pools_hdr[i];
        while(pd->next){
            size = hsms(pd->drl);
            
            hval = lookup(pd->key, pd->keyl, 0);
            hjval = jenkins_one_at_a_time_hash(pd->key, pd->keyl);

            HITEM_SWITCH((y=(hval&pools_htab->mask)));
            ph = pools_hitem[y];

            /* header is not user */
            for(; ph->next; ph=ph->next){
                if(hval == ph->hval &&
                    (pd->keyl == ph->keyl) &&
                    (hjval == ph->hjval) 
                    ){
                    MISS_LOCK();
                    pools_htab->miss++;
                    MISS_UNLOCK(); 
                    break;
                }
            }
             
            pd = pd->next;
            haddHitem(pd);
        }
    }

}		/* -----  end of function fetchdti  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  haddHitem
 *  Description:  hdr to hitem
 * =====================================================================================
 */
word haddHitem ( HDR *hdr ){   
    HITEM  *ph,*phtmp, *hp;
    ub4     y, _new_hval, _new_hjval;
    HSLAB *hs, *hsp;
    int i, m;

    if(hdr == NULL) return -1;
    if(hdr->skl > LIMIT_SLAB_BYTE) return -1;

    _new_hval = lookup(hdr->key,hdr->keyl,0);
    _new_hjval = jenkins_one_at_a_time_hash(hdr->key, hdr->keyl);
    HITEM_SWITCH((y=(_new_hval&pools_tab->mask)));

    ph = pools_hitem[y];    
    
    i = hsms(hdr->drl);
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
            
            if(m != i){  /* old size != new size , free old slab */
                addfslab(ph->psize, ph->sid, ph->sa);  /* free old slab */

                FSLAB *fslab = findslab(slabclass[i].size, i);
                ph->psize = slabclass[i].size;  /* update psize */
                hp->sid = fslab->sid;
                hp->sa = fslab->sa;
            }

            hsp = findhslab(i, ph->sid);    
            if(hsp != NULL){
                memcpy(hsp->sm+ph->sa*ph->psize, hdr->dr, hdr->drl);
                return 0;
            }else
                perror("hsp error\n");
            break;            
        }
        ph = ph->next; 
    } /* while */
    
    if(!phtmp->next){
        hp = hitemcreate();
        hp->key   = hdr->sk;
        hp->keyl  = hdr->skl;
        hp->drl = hdr->drl;
        hp->psize = slabclass[i].size;
        hp->hval  = hval;
        hp->hjval = hjval;
        
        FSLAB *fslab = findslab(hp->psize, i);
        HSLAB *hslab = findhslab(i, fslab->sid); 

        hp->sid = fslab->sid;
        hp->sa = fslab->sa;

        memcpy(hslab->sm+fslab->sa*hp->psize, hdr->dr, hdr->drl);

        phtmp->next = hp;

        pools_hitem_row[i]++;
        
    }

    hdr->flag = H_FALSE;

    /* make the hash table bigger if it is getting full */
    if (++pools_htab->count > (ub4)1<<(pools_htab->logsize))
    {
        hgrow();
        y = (x&t->mask);
    }
    
#ifdef HSANITY
    hsanity(t);
#endif  /* HSANITY */

    return TRUE;
}		/* -----  end of function haddHitem  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hrule
 *  Description:  
 * =====================================================================================
 */
void hrule ( HITEM *hitem ){
    int i

    if(pools_htab->bytes < conn_global->bytes){
            
        
        

        if(pools_htab->count < MAX_HARU_POOL){
            pools_haru_pool[pools_htab->count].hit = 0;
            pools_haru_pool[pools_htab->count].phitem = hitem;
            return;
        }else{

            if(pools_htab->count < pools_htab->logsize){
                /* MRU */    
                for (i=0 ; i<MAX_HARU_POOL && pools_haru_pool[i]!=NULL ; i++ ) {
                    if(pools_haru_pool[i].hit > hitem->ahit){

                    }
                }     
            }else{

            }
        }

    }
}		/* -----  end of function hrule  ----- */

 /* vim: set ts=4 sw=4: */

