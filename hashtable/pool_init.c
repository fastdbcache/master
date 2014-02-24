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
#include "pool_init.h"



/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hcreate
 *  Description:  
 * =====================================================================================
 */
void hcreate ( int isize ){
    int i, gcount;
    ub4 len;
    int max_slab, count_hslab;
    char cache_path[FILE_PATH_LENGTH];
    
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
    gcount  = pools_htab->gcount;
    if(gcount == 0)gcount = 1;
    
    for(i=0; i<gcount; i++){
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
        bzero(cache_path, FILE_PATH_LENGTH); 
        snprintf(cache_path, FILE_PATH_LENGTH-1, "%s/%s",conn_global->mmap_path, HashTable_for_list[5]);
        pools_fslab = (FSLAB *)mcalloc(max_slab, sizeof(FSLAB), cache_path, O_RDWR|O_CREAT);
    }
    else{
        pools_fslab = (FSLAB *)calloc(max_slab, sizeof(FSLAB));
    }  
    if(!pools_fslab){
        for(i=0; i< max_slab; i++){
            pools_fslab[i].sa = -1;
            pools_fslab[i].psize = 0;
        }
    }
    /*pthread_mutex_init(&work_lock_fslab, NULL);  */
    pthread_mutex_init(&work_lock_get, NULL);
    pthread_mutex_init(&work_lock_set, NULL);
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
    if(icount==0) icount++;
    pools_htab->bytes = 0;
    for(m=0; m<icount; m++){ 
        pools_hslab[m].sm = hslabcreate(m);        
        pools_hslab[m].ss = 0;
        pools_hslab[m].sf = conn_global->default_bytes;
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
        return NULL;
    }  
    if(conn_global->cache_method == D_MMAP){
        bzero(cache_path, FILE_PATH_LENGTH); 
        snprintf(cache_path, FILE_PATH_LENGTH-1, "%s/%s%05d",conn_global->mmap_path, HashTable_for_list[8], i);
        h = (ub1 *)mcalloc(1,conn_global->default_bytes ,cache_path,O_RDWR|O_CREAT);
        pools_htab->bytes += conn_global->default_bytes;
        if(pools_htab->lcount < (i+1)) {            
            pools_htab->lcount = i+1;
        }
        DEBUG("mmap %s", cache_path);
    }else{
        h = (ub1 *)calloc(conn_global->default_bytes, sizeof(ub1));
        pools_htab->bytes += conn_global->default_bytes;
        pools_htab->lcount = i+1;
        DEBUG("mem ");
    }
        
    if(!h){ 
        DEBUG("sm init error %d", conn_global->default_bytes);
        return NULL;
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
        
        bzero(cache_path, FILE_PATH_LENGTH); 
        snprintf(cache_path, FILE_PATH_LENGTH-1, "%s/%s%05d",conn_global->mmap_path, HashTable_for_list[7], i);
        hitem_group[i]->hrow = (HROW *)mcalloc(size , sizeof(HROW), cache_path, O_RDWR|O_CREAT);        
        if(!hitem_group[i]->hrow){
            DEBUG("hitem_group->hrow init error");
            exit(-1);
        }

        if(hitem_group[i]->bucket == 0){
            hitem_group[i]->bucket = size;
            hitem_group[i]->count = 0;
            hitem_group[i]->mask = size - 1;
            pools_htab->gcount = i+1;
        }
    }else{        
        hitem_group[i] = calloc(1, sizeof(HG));
        if(!hitem_group[i]){
            DEBUG("hitem_group init error");
            exit(-1);
        }
        hitem_group[i]->bucket = size;
        hitem_group[i]->count = 0;
        hitem_group[i]->mask = size - 1;
        hitem_group[i]->hrow = (HROW *)calloc(size, sizeof(HROW));
        if(!hitem_group[i]->hrow){
            DEBUG("hitem_group->hrow init error");
            exit(-1);
        }
        pools_htab->gcount = i + 1;
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
 *         Name:  hdrcreate
 *  Description:  
 * =====================================================================================
 */
HDR *hdrcreate (  ){
    HDR *d;
    d = (HDR *)calloc(1, sizeof(HDR));    
    if(d){
        d->key[0] = '\0';
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

