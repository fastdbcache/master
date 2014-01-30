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
 *         Name:  mmap_open
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
}		/* -----  end of function mmap_open  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  mmpo_init
 *  Description:  
 * =====================================================================================
 */
MMPO *mmpo_init (  ){
    MMPO *_mmpo;
    char *meta[]={"meta.sa", "meta.na"};
    char mmdb[FILE_PATH_LENGTH];
    uint32 val;
    void _meta;

    _mmpo = calloc(1, sizeof(MMPO));
    if(!_mmpo) return NULL;

    _mmpo->meta_sa = mmap_open( NULL, meta[0], sizeof(uint32)*4, O_RDWR|O_CREAT ); 
    _mmpo->meta_na = mmap_open( NULL, meta[1], sizeof(uint32)*4, O_RDWR|O_CREAT ); 
    _mmpo->mmdb_sa = NULL;
    _mmpo->mmdb_na = NULL;

    if(_mmpo->meta_sa != NULL){
        META_FID(_meta, _mmpo->meta_sa);
        memcpy(&val, _meta, sizeof(uint32));
        val = ntohl(val);
        if(val == 0){
            val = htonl(1);
            memcpy(_meta, &val, sizeof(uint32));
            META_UUID(_meta, _mmpo->meta_sa);
            memcpy(_meta, &val, sizeof(uint32));
            val = 1;
        }
        bzero(mmdb, FILE_PATH_LENGTH);
        snprintf(mmdb, FILE_PATH_LENGTH-1, "db.%010d", val);
        _mmpo->mmdb_sa = mmap_open(NULL, mmdb, conn_global->mmdb_length, O_RDWR|O_CREAT ); 
    }
    if(_mmpo->meta_na != NULL){
        META_FID(_meta, _mmpo->meta_na);
        memcpy(&val, _meta, sizeof(uint32));
        val = ntohl(val);
        if(val == 0){
            val = htonl(1);
            memcpy(_meta, &val, sizeof(uint32));
            META_UUID(_meta, _mmpo->meta_na);
            memcpy(_meta, &val, sizeof(uint32));
            val = 1;
        }
        bzero(mmdb, FILE_PATH_LENGTH);
        snprintf(mmdb, FILE_PATH_LENGTH-1, "db.%010d", val);
        _mmpo->mmdb_na = mmap_open(NULL, mmdb, conn_global->mmdb_length, O_RDWR); 
    }
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
    void *meta, *mmdb; 
    ub4  _lens;
    uint32 val, uuid, offset;
    char mmdb_name[FILE_PATH_LENGTH];

    if(!_dest){
        DEBUG("pools_dest is null");
        return -1;
    }
    if(keyl > (LIMIT_SLAB_BYTE)){
        DEBUG("keyl is too big %d", keyl);
        return -1;
    }

    _lens = alignByte(keyl);
    _mmpo = pools_dest->pool_mmpo;
    
    
    META_UUID(meta, _mmpo->meta_sa);
    memcpy(&val, meta, sizeof(uint32));
    uuid = ntohl(val);
    if(uuid == 9999999999){
        META_TOTAL(meta, _mmpo->meta_sa);
        memcpy(&val, meta, sizeof(uint32));
        val = ntohl(val);
        val = htonl(val+1);
        memcpy(meta, &val, sizeof(uint32));
        uuid = 1;
        val = htonl(uuid);
        memcpy(meta, &val, sizeof(uint32));
    }else
        val = htonl(uuid+1);
    
    memcpy(meta, &val, sizeof(uint32));    

    META_OFFSET(meta, _mmpo->meta_sa);
    memcpy(&val, meta, sizeof(uint32));
    offset = ntohl(val);
    if((offset + sizeof(uint32)*3 + _lens) > conn_global->mmdb_length){
        META_FID(meta, _mmpo->meta_sa);
        memcpy(&val, meta, sizeof(uint32));
        val = ntohl(val);
        
        bzero(mmdb_name, FILE_PATH_LENGTH);
        snprintf(mmdb_name, FILE_PATH_LENGTH-1, "db.%010d", val);
        _mmpo->mmdb_sa = mmap_open(NULL, mmdb_name, conn_global->mmdb_length, O_RDWR|O_CREAT )
        val = htonl(1+val);
        memcpy(meta, &val, sizeof(uint32));
        offset = 0;                
    }

    mmdb = _mmpo->mmdb_sa;
    mmdb += offset;

    val = htonl((uint32)get_sec());
    memcpy(mmdb+sizeof(uint32), &val, sizeof(uint32));
    memcpy(mmdb+sizeof(uint32)*2, key, keyl);
    memcpy(mmdb, &uuid, sizeof(uint32));
    
    offset += sizeof(uint32)*3+_lens;
    val = htonl(offset);
    META_OFFSET(meta, _mmpo->meta_sa);
    memcpy(meta, &val, sizeof(uint32));
    
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
    void *meta, *mmdb; 
    ub4  _lens;
    uint32 val, uuid, duuid, offset, len, etime;
    _ly *ply;

    if(!_dbp) return -1;

    _mmpo = pools_dest->pool_mmpo;
    mmdb = _mmpo->mmdb_na;
    
    META_UUID(meta, _mmpo->meta_na);
    memcpy(&val, meta, sizeof(uint32));
    uuid = ntohl(val);

    META_OFFSET(meta, _mmpo->meta_sa);
    memcpy(&val, meta, sizeof(uint32));
    offset = ntohl(val);
    mmdb += offset;    

    memcpy(&val, mmdb, sizeof(uint32));
    duuid = ntohl(val);
    if(duuid == 0)return -1;
    if(duuid != uuid){
        uuid = duuid;
    }
    
    memcpy(&len, mmdb+sizeof(uint32)*2+sizeof(char), sizeof(uint32));
    len = ntohl(len);
    len += sizeof(char);

    _dbp->inBuf = mmdb+sizeof(uint32)*2;
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
        
    etime = (uint32)get_sec();
    etime = htonl(etime);
    offset += sizeof(uint32)*2+len;
    memcpy(mmdb+sizeof(uint32)*2+len, etime, sizeof(uint32) );
    offset += sizeof(uint32);
    offset = htonl(offset);
    META_OFFSET(meta, _mmpo->meta_sa);
    memcpy(meta, &offset, sizeof(uint32));
 
    META_UUID(meta, _mmpo->meta_na);
    val = htonl(uuid+1);
    memcpy(meta, &val, sizeof(uint32));
    
    return 0;
}		/* -----  end of function mmap_pushdb  ----- */

 /* vim: set ts=4 sw=4: */


