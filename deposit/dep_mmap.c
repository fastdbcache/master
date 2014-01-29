/*
 * =====================================================================================
 *
 *       Filename:  dep_mmap.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/25/2014 01:59:18 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), vyouzhi@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "dep_mmap.h"


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  meta_open
 *  Description:  
 * =====================================================================================
 */
void mmap_open (void *start, char name, size_t byte, int flags ){
    int fd;
    char meta_name[FILE_PATH_LENGTH], null[1];
    void *p;

    bzero(meta_name, FILE_PATH_LENGTH);
    sprintf(meta_name, FILE_PATH_LENGTH-1, "%s/mmap.%s", conn_global->mmap_path, name);
    
    fd = open(meta_name, flags);
    if(fds == -1){
        DEBUG("open mmap.meta error");
        exit(-1);
    }
    
    p = mmap(start, byte, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(p == MAP_FAILED){
        DEBUG("init start_sa error");
        return NULL;
    }
    null[0]='\0';
    write(fd, null, 1);

    return p;
}		/* -----  end of function meta_open  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  mmpo_init
 *  Description:  
 * =====================================================================================
 */
MMPO *mmpo_init (  ){
    MMPO *_mmpo;
    int fds, fdn;
    char *meta_sa[]={"meta.sa", "meta.na"};
    char mmap_name_na[FILE_PATH_LENGTH];

    _mmpo = calloc(1, sizeof(MMPO));
    if(!_mmpo) return NULL;

    _mmpo->meta_sa = mmap_open( NULL, meta_sa[0], sizeof(uint32)*4, O_RDWR|O_CREAT ); 
    _mmpo->meta_sa = mmap_open( NULL, meta_sa[1], sizeof(uint32)*4, O_RDWR|O_CREAT ); 


    return _mmpo;
}		/* -----  end of function mmpo_init  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  mmap_init
 *  Description:  
 * =====================================================================================
 */
DEST *mmap_init ( size_t byte ){
    
    DEST *_dest;
    uint32 val;

    num = (int)(byte / (LIMIT_SLAB_BYTE));
    _dest = calloc(1, sizeof(DEST));
    if(!_dest) return NULL;

    _dest->total = num;
    _dest->maxbyte = byte;
    _dest->count = 1;
    _dest->isfull = H_FALSE;
    _dest->sd = 0;
    _dest->nd = 0;
    _dest->fe = H_USE;

        

    fdn = open(mmap_name_na, O_RDWR|O_CREAT);
    if(fdn == -1){
        DEBUG("open mmap.meta.na error");
        exit(-1);
    }
    _dest->start_na = mmap(NULL, sizeof(uint32)*5, PROT_READ|PROT_WRITE,MAP_SHARED, fdn, 0);
    if(_dest->start_na == MAP_FAILED){
        DEBUG("init start_na error");
        exit(-1);
    }
    _dest->doing = H_FALSE;
    _dest->pool_depo = NULL;
     
    return _dest;
}		/* -----  end of function mmap_init  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  mmap_set
 *  Description:  
 * =====================================================================================
 */
int mmap_set ( ub1 *key, ub4 keyl ){
    DEST *_dest = pools_dest;
    DEPO *_depo;
    ub4  _lens;

    if(!_dest){
        DEBUG("pools_dest is null");
        return -1;
    }
    if(keyl > (LIMIT_SLAB_BYTE)){
        DEBUG("keyl is too big %d", keyl);
        return -1;
    }

    _lens = keyl;
    if (_lens % CHUNK_ALIGN_BYTES)
            _lens += CHUNK_ALIGN_BYTES - (_lens % CHUNK_ALIGN_BYTES);

    
}		/* -----  end of function mmap_set  ----- */

 /* vim: set ts=4 sw=4: */


