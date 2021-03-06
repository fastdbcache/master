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
void hcreate ( int isize ){
    int i;
    ub4 len;
    int max_slab, count_hslab;
    char *cache_path;
    len = ((ub4)1<<isize);

    if(conn_global->cache_method == D_MMAP){
        pools_hfd = inithfd();
        
    }

    if(conn_global->cache_method == D_MMAP){ 
        
        cache_path = buildCachePath( HashTable_for_list[0] );
        pools_htab = (HTAB *)mcalloc(1, sizeof(HTAB), cache_path, O_RDWR|O_CREAT);
    }else{
        pools_htab = (HTAB *)calloc(1, sizeof(HTAB));
    }
    if(pools_htab == NULL){
        DEBUG("pools_htab calloc error");
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
    bzero(pools_htab->hslab_stat,  sizeof(pools_htab->hslab_stat));

    if(conn_global->cache_method == D_MMAP){
        
        
        cache_path = buildCachePath( HashTable_for_list[1] );
        pools_harug = (HARUG *)mcalloc(1, sizeof(HARUG)*sizeof(HARU)*MAX_HARU_POOL,cache_path,O_RDWR|O_CREAT);
    }else{
        pools_harug = (HARUG *)calloc(1, sizeof(HARUG));
    }
    pools_harug->step = 0;
    for(i=0; i < MAX_HARU_POOL; i++){
        /*pools_harug->pools_haru_pool[i] = (HARU *)calloc(1, sizeof(HARU));
        if(pools_harug->pools_haru_pool[i] == NULL){
            perror("pools_haru_pool callo error ");
            exit(1);
        }*/
        pools_harug->haru_pool[i].phitem = NULL;
    
    }
    pools_haru_pool = pools_harug->haru_pool;
   
    if(conn_global->cache_method == D_MMAP){

        
        
        cache_path = buildCachePath( HashTable_for_list[2] );
        pools_hitem_row = (ub4 *)mcalloc(len, sizeof(ub4),cache_path,O_RDWR|O_CREAT);
    }else{ 
        pools_hitem_row = calloc(len, sizeof(ub4));
    }

    hitem_group = (HG *)calloc(1, sizeof(HG));
    hitem_group->bucket = pools_htab->logsize;
    hitem_group->usable = inithitem ( (ub4) len );
    hitem_group->move = NULL;

    pools_hdr_head = hdrcreate();
    pools_hdr_tail = pools_hdr_head;

    if(conn_global->cache_method == D_MMAP){
        
        
        cache_path = buildCachePath( HashTable_for_list[3] );
        pools_tlist = (TLIST *)mcalloc(1, sizeof(TLIST)*sizeof(char)*KEY_LENGTH,cache_path,O_RDWR|O_CREAT);
    }else{
        pools_tlist = (TLIST *)calloc(1, sizeof(TLIST));
    }
    
    if(conn_global->cache_method == D_MMAP){
        conn_global->default_bytes = DEFAULT_MMAP_BYTE;
    }else{
        conn_global->default_bytes = DEFAULT_MEM_BYTE;
    }

    count_hslab = (int)( conn_global->maxbytes / conn_global->default_bytes );
    
    if(count_hslab < 1){
        count_hslab = 1; 
    }
    conn_global->chunk_bytes = count_hslab;

    inithslab ( count_hslab );

    max_slab = hslabclass();
    if(conn_global->cache_method == D_MMAP){
                
        cache_path = buildCachePath( HashTable_for_list[5] );
        pools_fslab = (FSLAB *)mcalloc(1, sizeof(FSLAB)*MAX_SLAB_CLASS, cache_path, O_RDWR|O_CREAT);
    }
    else{
        pools_fslab = (FSLAB *)calloc(max_slab, sizeof(FSLAB)*max_slab);
    }  
    for(i=0; i< MAX_SLAB_CLASS; i++){
        pools_fslab[i].psize = 0;
        pools_fslab[i].sid = 0;
        pools_fslab[i].sa = 0;    
    }

    conn_global->max_slab = max_slab;

    /*pthread_mutex_init(&work_lock_fslab, NULL);  */
    pthread_mutex_init(&work_lock_hit, NULL);
    pthread_mutex_init(&work_lock_miss, NULL);
    pthread_mutex_init(&work_lock_bytes, NULL);
    pthread_mutex_init(&work_lock_tlist, NULL);
    pthread_mutex_init(&work_lock_hdr, NULL);

    return ;
}		/* -----  end of function hcreate  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  inithslab
 *  Description:  
 * =====================================================================================
 */
void inithslab ( int i ){
    int m;
    char *cache_path;

    if(conn_global->cache_method == D_MMAP){
                
        cache_path = buildCachePath( HashTable_for_list[4] );
        pools_hslab = (HSLAB *)mcalloc(i, sizeof(HSLAB)*sizeof(char)*KEY_LENGTH,cache_path,O_RDWR|O_CREAT);
    }else{
        pools_hslab = (HSLAB *)calloc(i, sizeof(HSLAB));
    }

    for(m=0; m<i; m++){ 
        pools_hslab[m].sm=NULL;
        pools_hslab[m].ss=0;
        pools_hslab[m].sf=0;
        /*pools_hslab[m].id=0;  */
    }

}		/* -----  end of static function inithslab  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hslabnull
 *  Description:  
 * =====================================================================================
 */
HSLAB *hslabnull (  ){
    HSLAB *_h;
    _h = (HSLAB *)calloc(1, sizeof(HSLAB));
    if(_h != NULL){
        _h->sm = NULL;
        _h->ss = 0;
        _h->sf = 0;
       
    }
    return _h;
}		/* -----  end of function hslabnull  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hslabcreate
 *  Description:  
 * =====================================================================================
 */
HSLAB *hslabcreate ( int i ){
    HSLAB *h;

    h = hslabnull();
    h->sm = (ub1 *)calloc(LIMIT_SLAB_BYTE, sizeof(ub1));
    if(!h->sm)perror("h sm calloc error\n"); 
    h->sf = slabclass[i].chunk;
   
    pools_htab->bytes += LIMIT_SLAB_BYTE;    
    pools_htab->hslab_stat[i]++;
     
    return h;
}		/* -----  end of function hslabcreate  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hslabclass
 *  Description:  
 * =====================================================================================
 */
int hslabclass ( void ){
    int size = SLAB_BEGIN;
    int i=0;
    
    while (i < MAX_SLAB_CLASS && size <= MAX_SLAB_BYTE / conn_global->factor) {
        
        size = alignByte(size);
        /*if((item_size_max / size) < 2) break; 

        printf("slab class  chunk size %9u perslab %7u\n",
                     size, (MAX_SLAB_BYTE / size));*/
        slabclass[i].size = size;
        slabclass[i].chunk = (MAX_SLAB_BYTE / size);
        /*printf("slabclass: size:%d, chunk:%d\n", slabclass[i].size, slabclass[i].chunk);*/
        i++;
        size *= conn_global->factor;
    } 
    slabclass[i].size = MAX_SLAB_BYTE;
    slabclass[i].chunk = 1;

    return (++i);
}		/* -----  end of function hslabclass  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  inithitem
 *  Description:  
 * =====================================================================================
 */
HITEM **inithitem ( ub4 len ){
    int i;
    HITEM **hitem;
    hitem = (HITEM **)calloc(len, sizeof(HITEM));
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
    HITEM *h;
    h = (HITEM *)calloc(1, sizeof(HITEM));
    if(h){
        bzero(h->key, KEY_LENGTH);
        h->keyl = 0;
        h->drl = 0;
        h->psize = 0;
        h->sid = 0;
        h->sa = 0;
        h->hval = 0;
        h->hjval = 0;
        h->utime = 0;
        h->offtime = 0;
        h->ahit = 0;  
        h->amiss = 0;
        h->next = NULL;
    }
    return h;
}		/* -----  end of function hitemcreate  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  freehitem
 *  Description:  
 * =====================================================================================
 */
void freehitem ( HITEM **_h, ub4 len ){
    int i;

    if(!*_h) return;
    if(!_h) return;

    for(i=0; i<len; i++){
        if(!_h[i]) free(_h[i]);
    }

    free(*_h);

}		/* -----  end of function freehitem  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hdrcreate
 *  Description:  
 * =====================================================================================
 */
HDR *hdrcreate (  ){
    HDR *d;
    d = (HDR *)calloc(1, sizeof(HDR));
    if(d){
        d->key = NULL;
        d->keyl = 0;
        d->stime = 0;
        d->flag = H_TRUE;
        d->dr = NULL;
        d->drl = 0;
        d->next = NULL;            
    }
    return d;
}		/* -----  end of function hdrcreate  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  initulist
 *  Description:  
 * =====================================================================================
 */
ULIST *initulist (  ){
    ULIST *u;

    u = (ULIST *)calloc(1, sizeof(ULIST));
    if(u){
        bzero(u->key, KEY_LENGTH) ;
        u->keyl = 0;
        u->utime = 0;
        u->flag = H_TRUE;
        u->next = NULL;
    }
    
    return u;
}		/* -----  end of function initulist  ----- */

 /* vim: set ts=4 sw=4: */

