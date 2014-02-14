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
    int i, m;
    ub4 len;
    int max_slab, count_hslab;
    char cache_path[FILE_PATH_LENGTH];
    HROW *_hrow;
    
    if(conn_global->cache_method == D_MMAP){
        pools_hfd = inithfd();
    }

    if(conn_global->cache_method == D_MMAP){ 
        bzero(cache_path, FILE_PATH_LENGTH); 
        snprintf(cache_path, FILE_PATH_LENGTH-1, "%s/%s",conn_global->mmap_path, HashTable_for_list[0]);
        pools_htab = (HTAB *)mcalloc(1, sizeof(HTAB), cache_path, O_RDWR|O_CREAT);
    }else{
        pools_htab = (HTAB *)calloc(1, sizeof(HTAB));
    }
    if(pools_htab == NULL){
        DEBUG("pools_htab calloc error");
        exit(1);
    }
    if(pools_htab->gcount == 0)pools_htab->gcount = 1;
    if(pools_htab->lcount == 0)pools_htab->lcount = 1;

    if(conn_global->cache_method == D_MMAP){                
        bzero(cache_path, FILE_PATH_LENGTH); 
        snprintf(cache_path, FILE_PATH_LENGTH-1, "%s/%s",conn_global->mmap_path, HashTable_for_list[1]);
        pools_harug = (HARUG *)mcalloc(1, sizeof(HARUG),cache_path,O_RDWR|O_CREAT);
    }else{
        pools_harug = (HARUG *)calloc(1, sizeof(HARUG));
    }
    if(!pools_harug){
        DEBUG("pools_harug init error");
        exit(-1);
    }
    pools_harug->step = 0;
    bzero(pools_harug->haru_pool, MAX_HARU_POOL);
    
    pools_haru_pool = pools_harug->haru_pool;
  
    /* 
    if(conn_global->cache_method == D_MMAP){
        bzero(cache_path, FILE_PATH_LENGTH); 
        snprintf(cache_path, FILE_PATH_LENGTH-1, "%s/%s",conn_global->mmap_path, HashTable_for_list[2]);
        pools_hitem_row = (ub4 *)mcalloc(len, sizeof(ub4), cache_path,O_RDWR|O_CREAT);
    }else{ 
        pools_hitem_row = calloc(len, sizeof(ub4));
    }  */

    bzero(hitem_group, MAX_HG_LENGTH);
    for(i=0; i<pools_htab->gcount; i++){
        len = (ub4)isize<<i;
        initHitemGroup ( len, i );
    }
    if(!hitem_group[0]->hrow){
        DEBUG("init hrow error");
        exit(-1);
    }
    
    pools_hdr_head = hdrcreate();
    pools_hdr_tail = pools_hdr_head;

    /*if(conn_global->cache_method == D_MMAP){
        
        bzero(cache_path, FILE_PATH_LENGTH); 
        snprintf(cache_path, FILE_PATH_LENGTH-1, "%s/%s",conn_global->mmap_path, HashTable_for_list[3]);
        pools_tlist = (TLIST *)mcalloc(1, sizeof(TLIST),cache_path,O_RDWR|O_CREAT);
    }else{  */
        pools_tlist = (TLIST *)calloc(1, sizeof(TLIST));
   // }
    if(!pools_tlist) {
        DEBUG("pools_tlist is null");
        exit(-1);
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
    conn_global->max_slab = max_slab;
    if(conn_global->cache_method == D_MMAP){
        max_slab = (int)(( conn_global->maxbytes / SLAB_BEGIN )/2);
        bzero(cache_path, FILE_PATH_LENGTH); 
        snprintf(cache_path, FILE_PATH_LENGTH-1, "%s/%s",conn_global->mmap_path, HashTable_for_list[5]);
        pools_fslab = (FSLAB *)mcalloc(max_slab, sizeof(FSLAB), cache_path, O_RDWR|O_CREAT);
    }
    else{
        pools_fslab = (FSLAB *)calloc(max_slab, sizeof(FSLAB));
    }  
    
    

    /*pthread_mutex_init(&work_lock_fslab, NULL);  */
    pthread_mutex_init(&work_lock_hit, NULL);
    pthread_mutex_init(&work_lock_miss, NULL);
    pthread_mutex_init(&work_lock_bytes, NULL);
    pthread_mutex_init(&work_lock_tlist, NULL);
    pthread_mutex_init(&work_lock_hdr, NULL);

    DEBUG("init ok");
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
    ssize_t icount;
    char cache_path[FILE_PATH_LENGTH];

    if(conn_global->cache_method == D_MMAP){
        bzero(cache_path, FILE_PATH_LENGTH); 
        snprintf(cache_path, FILE_PATH_LENGTH-1, "%s/%s",conn_global->mmap_path, HashTable_for_list[4]);
        pools_hslab = (HSLAB *)mcalloc(i, sizeof(HSLAB),cache_path,O_RDWR|O_CREAT);
    }else{
        pools_hslab = (HSLAB *)calloc(i, sizeof(HSLAB));
    }

    icount = pools_htab->lcount;
    for(m=0; m<icount; m++){ 
        pools_hslab[m].sm = hslabcreate(m);        
    }

}		/* -----  end of static function inithslab  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hslabcreate
 *  Description:  
 * =====================================================================================
 */
ub1 *hslabcreate ( int i ){
    ub1 *h;
    char cache_path[FILE_PATH_LENGTH];

    if( pools_htab->bytes >= conn_global->maxbytes ){
        DEBUG("not any momey for user bytes:%d,maxbytes: %d", pools_htab->bytes, conn_global->maxbytes);
        return -1;
    }  
    if(conn_global->cache_method == D_MMAP){
        bzero(cache_path, FILE_PATH_LENGTH); 
        snprintf(cache_path, FILE_PATH_LENGTH-1, "%s/%s%05d",conn_global->mmap_path, HashTable_for_list[8], i);
        h = (HSLAB *)mcalloc(1,conn_global->default_bytes ,cache_path,O_RDWR|O_CREAT);
        DEBUG("mmap %s", cache_path);
    }else{
        h = (ub1 *)calloc(conn_global->default_bytes, sizeof(ub1));
        pools_htab->bytes += conn_global->default_bytes;
        pools_htab->lcount = i;
        DEBUG("mem ");
    }
    
    if(!h){ 
        DEBUG("sm init error %d", conn_global->default_bytes);
        return -1;
    } 
    return h;
}		/* -----  end of function hslabcreate  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  initHitemGroup
 *  Description:  
 * =====================================================================================
 */
void initHitemGroup ( ub4 size, int i ){
    char cache_path[FILE_PATH_LENGTH];
    if(conn_global->cache_method == D_MMAP){
        bzero(cache_path, FILE_PATH_LENGTH); 
        snprintf(cache_path, FILE_PATH_LENGTH-1, "%s/%s%05d",conn_global->mmap_path, HashTable_for_list[6], i);
        hitem_group[i] = (HG *)mcalloc(1, sizeof(HG),cache_path,O_RDWR|O_CREAT);
        if(!hitem_group[i]){
            DEBUG("hitem_group init error");
            exit(-1);
        }
        if(hitem_group[i]->bucket == 0){
            hitem_group[i]->bucket = size;
            hitem_group[i]->count = 0;
            hitem_group[i]->mask = hitem_group[i]->bucket-1;
        }
        bzero(cache_path, FILE_PATH_LENGTH); 
        snprintf(cache_path, FILE_PATH_LENGTH-1, "%s/%s%05d",conn_global->mmap_path, HashTable_for_list[7], i);
        hitem_group[i]->hrow = (HROW *)mcalloc(hitem_group[i]->bucket, sizeof(HROW), cache_path, O_RDWR|O_CREAT);        
        if(!hitem_group[i]->hrow){
            DEBUG("hitem_group->hrow init error");
            exit(-1);
        }
    }else{
        hitem_group[i] = calloc(1, sizeof(HG));
        if(!hitem_group[i]){
            DEBUG("hitem_group init error");
            exit(-1);
        }
        hitem_group[i]->bucket = size;
        hitem_group[i]->count = 0;
        hitem_group[i]->mask = hitem_group[i]->bucket-1;
        hitem_group[i]->hrow = (HROW *)calloc(hitem_group[i]->bucket, sizeof(HROW));        
        if(!hitem_group[i]->hrow){
            DEBUG("hitem_group->hrow init error");
            exit(-1);
        }
    }
}		/* -----  end of function initHitemGroup  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hslabclass
 *  Description:  
 * =====================================================================================
 */
int hslabclass ( void ){
    int size = SLAB_BEGIN;
    int i=0;
   
    bzero(slabclass, MAX_SLAB_CLASS); 
    while (i < MAX_SLAB_CLASS && size <= LIMIT_SLAB_BYTE / conn_global->factor) {
        
        size = alignByte(size);
        /*if((item_size_max / size) < 2) break; 

        printf("slab class  chunk size %9u perslab %7u\n",
                     size, (LIMIT_SLAB_BYTE / size));*/
        slabclass[i].size = size;
        slabclass[i].chunk = (LIMIT_SLAB_BYTE / size);
        /*printf("slabclass: size:%d, chunk:%d\n", slabclass[i].size, slabclass[i].chunk);*/
        i++;
        size *= conn_global->factor;
    } 
    slabclass[i].size = LIMIT_SLAB_BYTE;
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

