/*
 * =====================================================================================
 *
 *       Filename:  hashtab.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/25/2013 06:00:08 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), 
 *   Organization:  
 *
 * =====================================================================================
 */


#ifndef HASHTAB
#include "hashtab.h"
#endif

/*
 * hgrow - Double the size of a hash table.
 * Allocate a new, 2x bigger array,
 * move everything from the old array to the new array,
 * then free the old array.
 */
void hgrow(int i)
{
    ub4 len;
    len = hitem_group[i-1].bucket;
    len = len << 1; 

    initHitemGroup(len, i);      
}



/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  findhslab
 *  Description:  
 * =====================================================================================
 */
HSLAB *findhslab ( ssize_t i, sb2 _sid){
/*    HSLAB *slab, *p;

    slab = pools_hslab[i];

    for(p=slab; p; p=p->next){
        if(p->id == _sid){             
            return p;
        }
    }  */

    return NULL;
}		/* -----  end of function findhslab  ----- */





/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hsms
 *  Description:  
 * =====================================================================================
 */
int hsms ( ub4 bytes ){
    int i;

    if(bytes > conn_global->default_bytes){
        DEBUG("big bytes");
        return -1;
    }

    i = 0;
    
    for ( ; slabclass[i].chunk > 0; i++ ) {
        if(slabclass[i].size >= bytes) return i;
    }
    return -1;
}		/* -----  end of function hsms  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  addfslab
 *  Description:  
 * =====================================================================================
 */
void addfslab ( HITEM *_ph){
    int i; 
    uint32 pre_sid, pre_sa;
    char cache_path[FILE_PATH_LENGTH];

    if(!_ph) return;
    i = hsms(_ph->psize);

    if(pools_hslab[_ph->sid].sm == NULL){
        bzero(cache_path, FILE_PATH_LENGTH); 
        snprintf(cache_path, FILE_PATH_LENGTH-1, "%s/%s%05d",conn_global->mmap_path, HashTable_for_list[8], _ph->sid);
        pools_hslab[_ph->sid].sm = (HSLAB *)mcalloc(1,conn_global->default_bytes ,cache_path,O_RDWR|O_CREAT);
    }

    pre_sa = htonl(pools_fslab[i].sa);    
    memcpy(pools_hslab[_ph->sid].sm+_ph->sa, &pre_sa, sizeof(uint32));
    pre_sid = htonl(pools_fslab[i].sid);
    memcpy(pools_hslab[_ph->sid].sm+_ph->sa+sizeof(uint32), &pre_sid, sizeof(uint32));
            
    pools_fslab[i].psize = _ph->psize;
    pools_fslab[i].sa = _ph->sa;
    pools_fslab[i].sid = _ph->sid;
        
    return;
}		/* -----  end of static function addfslab  ----- */

f
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  freefslab
 *  Description:  
 * =====================================================================================
 */
void freefslab ( int i ){
    uint32 pre_sa, pre_sid;
    char cache_path[FILE_PATH_LENGTH];
    sb2 sid;
    ub4 sa;
    
    sid = pools_fslab[i].sid;
    sa = pools_fslab[i].sa;
     
    if(pools_hslab[sid].sm == NULL){
        bzero(cache_path, FILE_PATH_LENGTH); 
        snprintf(cache_path, FILE_PATH_LENGTH-1, "%s/%s%05d",conn_global->mmap_path, HashTable_for_list[8], _ph->sid);
        pools_hslab[sid].sm = (HSLAB *)mcalloc(1,conn_global->default_bytes ,cache_path,O_RDWR|O_CREAT);
    }
    memcpy(&pre_sa, pools_hslab[sid].sm+sa, sizeof(uint32));
    pre_sa = ntohl(pre_sa);
    memcpy(&pre_sid, pools_hslab[sid].sm+sa+sizeof(uint32), sizeof(uint32));
    pre_sid = ntohl(pre_sid);

    pools_fslab[i].sa = pre_sa;
    pools_fslab[i].sid = pre_sid;
     
}		/* -----  end of function freefslab  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  findfslab
 *  Description:  
 * =====================================================================================
 */
FSLAB *findfslab ( ub4 _psize ){
    /*int i;

    i = hsms(_psize);

    if(pools_fslab[i]->sa != 0){
        return i;
    }

    
    FSLAB *fslab, *res;
    
    fslab = pools_fslab;

    do {
        res = fslab;
        fslab = fslab->next;
        if(fslab == NULL) return NULL;
        if(fslab->psize == _psize){
            res->next = fslab->next;
            return fslab;
        }
    } while ( 1 );				 -----  end do-while  ----- */
    
    return NULL;
}		/* -----  end of function findfslab  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  findslab
 *  Description:  
 * =====================================================================================
 */
int findslab ( ub4 _psize){
    HSLAB *hslab, *hs_tmp;
    int i;
    char cache_path[FILE_PATH_LENGTH];

    if(_psize == -1){ 
        return -1;
    }

    if(_psize > conn_global->default_bytes) return -1;

    for(i=0; i<conn_global->chunk_bytes; i++){
        hslab = pools_hslab+i;
        if(!hslab) return -1;

        if( hslab->sm == NULL ){          
            
            hslab->sm = hslabcreate(i); 
            
            if(!hslab->sm){ 
                DEBUG("sm init error %d", conn_global->default_bytes);
                return -1;
            }
            hslab->ss = 0;
            hslab->sf = conn_global->default_bytes;            
        }

        if(hslab->sf > _psize){            
            hslab->ss += _psize;
            hslab->sf -= _psize;
            return i;
        }else{
            DEBUG("sf:%llu, psize:%llu", hslab->sf, _psize);
        }
    }
    
    return -1;     
}		/* -----  end of function findslab  ----- */


 /* vim: set ts=4 sw=4: */

