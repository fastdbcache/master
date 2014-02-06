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
 *         Name:  mmpo_init
 *  Description:  
 * =====================================================================================
 */
MMPO *mmpo_init (  ){
    MMPO *_mmpo;   
    char meta[]="mmpo.meta";
    char mmdb[FILE_PATH_LENGTH];

    bzero(mmdb, FILE_PATH_LENGTH); 
    snprintf(mmdb, FILE_PATH_LENGTH-1, "%s/%s",conn_global->mmap_path, meta);
              
    _mmpo = (MMPO *) mcalloc(2, sizeof(MMPO), mmdb, O_RDWR|O_CREAT);

    return _mmpo;
}		/* -----  end of function mmpo_init  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  mmap_init
 *  Description:  
 * =====================================================================================
 */
DEST *mmap_init ( size_t byte ){
    int num;
    DEST *_dest;
    uint32 val;
    
    num = (int)(byte / (LIMIT_SLAB_BYTE));
    _dest = (DEST *)calloc(1, sizeof(DEST));
    if(!_dest) return NULL;

    _dest->total = num;
    _dest->maxbyte = byte;
    _dest->count = 1;
    _dest->isfull = H_FALSE;
    _dest->sd = 0;
    _dest->nd = 0;
    _dest->fe = H_USE;

    _dest->pool_mmpo = mmpo_init();
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
    MMPO *_mmpo;
    ub4  _lens;
    uint32 val, uuid, offset;
    char mmdb_name[FILE_PATH_LENGTH], *mmdb;
    
    if(!pools_dest->pool_mmpo) return -1;

    if(keyl > (LIMIT_SLAB_BYTE)){
        DEBUG("keyl is too big %d", keyl);
        return -1;
    }

    _lens = alignByte(keyl);

    _mmpo = pools_dest->pool_mmpo;
       
    if((_mmpo->offset + sizeof(uint32)*3 + _lens) > conn_global->mmdb_length){
        _mmpo->id++; 
        
        bzero(mmdb_name, FILE_PATH_LENGTH);
        snprintf(mmdb_name, FILE_PATH_LENGTH-1, "%s/db.%010d",conn_global->mmap_path, _mmpo->id);
        
        _mmpo->offset = 0;                
        pools_mmap[0] = (char *)mcalloc(1, sizeof(char)*DEFAULT_MMAP_BYTE, mmdb_name, O_RDWR|O_CREAT);
    }

    mmdb = pools_mmap[0];
    mmdb += _mmpo->offset;

    val = htonl((uint32)get_sec());
    memcpy(mmdb+sizeof(uint32), &val, sizeof(uint32));
    memcpy(mmdb+sizeof(uint32)*2, key, keyl);
    uuid = htonl(_mmpo->uuid++);    
    memcpy(mmdb, &uuid, sizeof(uint32));
    
    _mmpo->offset += sizeof(uint32)*3+_lens;
        
    return 0;
}		/* -----  end of function mmap_set  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  mmap_pushdb
 *  Description:  
 * =====================================================================================
 */
int mmap_pushdb ( DBP *_dbp ){
    MMPO *_mmpo;
    uint32 val, uuid, len, etime, utime;
    _ly *ply;
    char mmdb_name[FILE_PATH_LENGTH], *mmdb;

    if(!_dbp) return -1;

    _mmpo = pools_dest->pool_mmpo+1;
    if(!_mmpo) {
        DEBUG("mmpo error");
        return -1;
    }
    mmdb = pools_mmap[1];
    mmdb += _mmpo->offset;
  
    memcpy(&val, mmdb, sizeof(uint32));
    uuid = ntohl(val);
    if(uuid == 0){
        if(pools_dest->pool_mmpo[0].id == _mmpo->id) return -1;        
        _mmpo->id++;
        bzero(mmdb_name, FILE_PATH_LENGTH);
        snprintf(mmdb_name, FILE_PATH_LENGTH-1, "db.%010d", _mmpo->id);
        pools_mmap[1] = (char *)mcalloc(1, sizeof(char)*DEFAULT_MMAP_BYTE, mmdb_name, O_RDWR);
        _mmpo->offset = 0; 
        mmdb = pools_mmap[1];
        mmdb += _mmpo->offset;
    }
            
    memcpy(&len, mmdb+sizeof(uint32)*2+sizeof(char), sizeof(uint32));
    len = ntohl(len);
    len += sizeof(char);

    _dbp->inBuf = calloc(len, sizeof(char));
    memcpy(_dbp->inBuf, mmdb+sizeof(uint32)*2, len);
    _dbp->inEnd = len;
    _dbp->inBufSize = len;
    ply = parser_do ((char *)(_dbp->inBuf+sizeof(char)+sizeof(uint32)), len-sizeof(char)-sizeof(uint32));

    if(!ply){
        /*  DEBUG("ply is null %s", _u->key);*/
        DEBUG("ply is null");
        return -1;
    } 
    /*  update tlist  */
    utime = get_sec();
    pushList((ub1 *)ply->tab, ply->len, utime);

    len = alignByte(len);
        
    etime = htonl((int)utime);
    _mmpo->offset += sizeof(uint32)*2+len;
    memcpy(mmdb+sizeof(uint32)*2+len, &etime, sizeof(uint32) );
    _mmpo->offset += sizeof(uint32);
    
    _mmpo->uuid++;    

    return 0;
}		/* -----  end of function mmap_pushdb  ----- */

 /* vim: set ts=4 sw=4: */
