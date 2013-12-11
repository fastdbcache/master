/*
 * =====================================================================================
 *
 *       Filename:  pool_init.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/11/2013 10:05:41 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "pool_init.h"

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hcreate
 *  Description:  
 * =====================================================================================
 */
void hcreate ( work isize ){
    int i;
    ub4 len;
    len = ((ub4)1<<isize);

    pools_htab = (HTAB *)calloc(1, sizeof(HTAB));
    if(pools_htab == NULL){
        perror("pools_htab calloc error");
        exit(1);
    }
    pools_htab->mask = len - 1;
    pools_htab->logsize = len;
    pools_htab->count = 0;
    pools_htab->bcount = 0;
    pools_htab->lcount = 0;
    pools_htab->hit = 0;
    pools_htab->miss = 0;
    pools_htab->set = 0;
    pools_htab->get = 0;
    pools_htab->bytes = 0;
    bzero(pools_htab->hslab_stat, 0, sizeof(pools_htab->hslab_stat));

    for(i=0; i < MAX_HARU_POOL; i++){
        pools_haru_pool[i] = (HARU *)calloc(1, sizeof(HARU));
        if(pools_haru_pool[i] == NULL){
            perror("pools_haru_POOL callo error ");
            exit(1);
        }
    
    }
    pools_hitem = inithitem ( (ub4) len )
    
    pools_hdr = inithdr();

    pools_tlist = (TLIST *)calloc(1, sizeof(TLIST));

    pools_ulist = initulist();

    int max_slab = hslabclass();

    if(max_slab > 0){
        inithslab ( max_slab );
    }

    pthread_mutex_init(&work_lock_fslab, NULL);
    pools_fslab = (FSLAB *)calloc(1, sizeof(FSLAB));
    pools_fslab->psize = 0;
    pools_fslab->sid = 0;
    pools_fslab->sa = 0;    
    pools_fslab->next = NULL;
    return ;
}		/* -----  end of function hcreate  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  inithslab
 *  Description:  
 * =====================================================================================
 */
static void inithslab ( int i ){
    int m;
    pools_hslab = (HSLAB *)calloc(i, sizeof(HSLAB));
    for(m=0; m<i; m++){ 
        pools_hslab[m] = (HSLAB *)calloc(1, sizeof(HSLAB));
        if(pools_hslab[m] != NULL){
            pools_hslab[m]->sm = NULL;
            pools_hslab[m]->ss = 0;
            pools_hslab[m]->sf = 0;
            pools_hslab[m]->id = 0;
            pools_hslab[m]->next = NULL;
        }
    }

}		/* -----  end of static function inithslab  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  inithitem
 *  Description:  
 * =====================================================================================
 */
HITEM **inithitem ( ub4 len ){
    int i;
    HITEM **hitem
    hitem = (HITEM *)calloc(len, sizeof(HITEM));
    if(hitem == NULL) return NULL;
    /* pools_hitem head not store anything */
    for (i=0; i<len; ++i) {
        hitem[i] = hitemcreate();
    }
    return hitem;
}		/* -----  end of function inithitem  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hitemcreate
 *  Description:  
 * =====================================================================================
 */

HITEM *hitemcreate(){
    HITEM *h
    h = (HITEM *)calloc(1, sizeof(HITEM));
    if(h){
        h->key = NULL;
        h->keyl = 0;
        h->drl = 0;
        h->psize = 0;
        h->sid = 0;
        h->sa = 0;
        h->hval = 0;
        h->hjval = 0;
        h->utime = 0;
        h->ahit = 0;  
        h->next = NULL;
    }
    return h;
}		/* -----  end of function hitemcreate  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  inithdr
 *  Description:  
 * =====================================================================================
 */
HDR **inithdr (  ){
    int i;
    HDR **d;
    d = (HDR *) calloc(conn_global->process_num, sizeof(HDR)); 
    
    for(i=0; i<conn_global->process_num; i++){
        d[i] = (HDR *)calloc(1, sizeof(HDR));
        if(d[i]){
            d[i]->key = NULL;
            d[i]->keyl = 0;
            d[i]->stime = 0;
            d[i]->flag = 0;
            d[i]->dr = NULL;
            d[i]->drl = 0;
            d[i]->pid = 0;
            d[i]->next = NULL;            
        }
    }
    return d; 
}		/* -----  end of function inithdr  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  initulist
 *  Description:  
 * =====================================================================================
 */
ULIST **initulist (  ){
    ULIST **u;
    int i;
    u = (ULIST *)calloc(conn_global->process_num, sizeof(ULIST));
    for(i=0; i<conn_global->process_num; i++){
        u[i] = (TLIST *)calloc(1, sizeof(TLIST));
        if(u[i]){
            u[i]->name = NULL;
            u[i]->utime = 0;
            u[i]->flag = 0;
            u[i]->next = NULL;
        }
    }
    return u;
}		/* -----  end of function initulist  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hslabcreate
 *  Description:  
 * =====================================================================================
 */
HSLAB *hslabcreate ( ssize_t chunk ){
    HSLAB *h;
    h = (HSLAB *)calloc(1, sizeof(HSLAB));
    h->sm = (char *)calloc(MAX_SLAB_BYTE, sizeof(char));;
    h->ss = 0;
    h->sf = chunk;
    h->id = 0;
    h->next = NULL;
    return h;
}		/* -----  end of function hslabcreate  ----- */

 /* vim: set ts=4 sw=4: */

