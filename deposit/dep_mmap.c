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

#include "dep_mmap.h"

uint32 last_mmpo_id = 0;
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  mmpo_init
 *  Description:  
 * =====================================================================================
 */
MMPO *mmpo_init (  ){
    MMPO *_mmpo, *_mmpo_des;
    char meta[]="mmpo.meta";
    char mmdb[FILE_PATH_LENGTH];
    int i;

    bzero(mmdb, FILE_PATH_LENGTH); 
    snprintf(mmdb, FILE_PATH_LENGTH-1, "%s/%s",conn_global->mmap_path, meta);
              
    _mmpo = (MMPO *) mcalloc(2, sizeof(MMPO), mmdb, O_RDWR|O_CREAT);
    _mmpo_des = _mmpo;

    pools_mmap[0] = (char *)mmapdb(_mmpo->id);    
    
    _mmpo = _mmpo+1;
    pools_mmap[1] = (char *)mmapdb(_mmpo->id);
    for(i=0; i<2; i++){
        if(!pools_mmap[i]){
            DEBUG("pools_mmap init error %d",i);
            exit(-1);
        }
    }
    return _mmpo_des;
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
   
    num = byte / (DEFAULT_MMAP_BYTE);
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
    char *mmdb;
    
    if(!pools_dest->pool_mmpo){
        DEBUG("pool_mmpo is null");
        return -1;
    }

    if(keyl > (LIMIT_SLAB_BYTE)){
        DEBUG("keyl is too big %d", keyl);
        return -1;
    }

    _lens = alignByte((keyl+sizeof(uint32)*3));
    DEPO_LOCK();
    _mmpo = pools_dest->pool_mmpo;
     
    if((_mmpo->offset + _lens) > conn_global->mmdb_length){

        if(_mmpo->offset + sizeof(uint32) >= conn_global->mmdb_length){
            val = htonl(0);
            mmdb = pools_mmap[0];
            if(mmdb){
                memcpy(mmdb+_mmpo->offset, &val, sizeof(uint32));
            }
            else{
                DEBUG("error");
                DEPO_UNLOCK();
                return -1;
            }
        }
        //if(_mmpo->id != pools_dest->pool_mmpo[1].id)
        //    unmmapdb(pools_mmap[0], _mmpo->id);         
        _mmpo->offset = 0;        
         _mmpo->id++;   
        pools_mmap[0] = (char *)mmapdb(_mmpo->id);
        if(!pools_mmap[0]){
            DEBUG("pools_mmap init error");
            DEPO_UNLOCK();
            return -1;
        }else{
            DEBUG("uuid:%llu", _mmpo->uuid);
        }

    }

    mmdb = pools_mmap[0];
    if(!mmdb){
        DEBUG("mmdb is null");
        DEPO_UNLOCK();
        return -1;
    }
    mmdb += _mmpo->offset;

    val = htonl((int)get_sec());
    memcpy(mmdb+sizeof(uint32), &val, sizeof(uint32));
    memcpy(mmdb+sizeof(uint32)*2, key, keyl);
    
    uuid = htonl(++_mmpo->uuid);    
    memcpy(mmdb, &uuid, sizeof(uint32));
    
    _mmpo->offset += _lens;
    DEPO_UNLOCK();
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

    if(!_dbp){
        DEBUG("dbp is null");
        return -1;
    }

    _mmpo = pools_dest->pool_mmpo+1;
    if(!_mmpo) {
        DEBUG("mmpo error");
        return -1;
    }
            
    if( _mmpo->offset+sizeof(uint32)  >= conn_global->mmdb_length ){                
        _mmpo->id++;        
        pools_mmap[1] =(char *)mmapdb(_mmpo->id);
        if(!pools_mmap[1]){
            DEBUG("_mmpo->id:%d", _mmpo->id);
            return -1;
        }
        _mmpo->offset = 0; 
    }
    
    found_mmdb:
        mmdb = pools_mmap[1];
        mmdb += _mmpo->offset;
        memcpy(&val, mmdb, sizeof(uint32));
        uuid = ntohl(val);

        if(uuid == 0 ){
            unmmapdb();
            if(pools_dest->pool_mmpo[0].id > _mmpo->id){
                _mmpo->id++;        
                pools_mmap[1] =(char *)mmapdb(_mmpo->id);
                if(!pools_mmap[1]){
                    DEBUG("_mmpo->id:%d", _mmpo->id);
                    return -1;
                }
                _mmpo->offset = 0;
                goto found_mmdb;     
            }else{
                
                DEBUG("pushdb id eq");                
                return -1;
            }
        }
    DEBUG("id:%d, uuid:%llu", _mmpo->id, _mmpo->uuid);
    memcpy(&len, mmdb+sizeof(uint32)*2+sizeof(char), sizeof(uint32));
    len = ntohl(len);
    len += sizeof(char);

    /*_dbp->inBuf = calloc(len, sizeof(char));
    if(!_dbp->inBuf){
        DEBUG("inbuf is null");
        return -1;
    }  */
    if(CheckBufSpace(len, _dbp) != 0){
        DEBUG("CheckBufSpace error");
        return -1;
    }
    memcpy(_dbp->inBuf, mmdb+sizeof(uint32)*2, len);
    
    ply = parser_do (_dbp->inBuf+sizeof(char)+sizeof(uint32), len-sizeof(char)-sizeof(uint32));

    if(!ply){
        /*  DEBUG("ply is null %s", _u->key);*/
        DEBUG("ply is null");
        return -1;
    } 
    /*  update tlist  */
    utime = get_sec();
    /*DEBUG("pushdb len:%d, tab:%s", ply->len, ply->tab);  */
    pushList((ub1 *)ply->tab, ply->len, utime);

    etime = htonl((int)utime);
    memcpy(mmdb+sizeof(uint32)*2+len, &etime, sizeof(uint32) );
    _mmpo->offset += alignByte(len+sizeof(uint32)*3);
    
    _mmpo->uuid++;    

    return 0;
}		/* -----  end of function mmap_pushdb  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  mmapdb
 *  Description:  
 * =====================================================================================
 */
void *mmapdb ( int id ){
    void *pdb;
    MMPO *_mmpo;
    ub4  _lens;
    uint32 val, uuid, offset;
    char mmdb_name[FILE_PATH_LENGTH], *mmdb;
        
    bzero(mmdb_name, FILE_PATH_LENGTH);
    snprintf(mmdb_name, FILE_PATH_LENGTH-1, "%s/mmpo.db.%010d",conn_global->mmap_path, id);
    
    DEBUG("mmdb_name:%s", mmdb_name);
    pdb = mcalloc(1, conn_global->mmdb_length, mmdb_name, O_RDWR|O_CREAT);
    if(!pdb){
        DEBUG("pools_mmap mcalloc error");
        return NULL;
    }

    return pdb;
}		/* -----  end of function mmapdb  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  unmmapdb
 *  Description:  
 * =====================================================================================
 */
void unmmapdb (){
    char mmdb_name[FILE_PATH_LENGTH], *mmdb;
    uint32 id;
    MMPO *_mmpo;

    _mmpo = pools_dest->pool_mmpo+1;
    if(!_mmpo) {
        DEBUG("mmpo error");
        return -1;
    }
    id = last_mmpo_id;
    for(; id<_mmpo->id; id++){
        bzero(mmdb_name, FILE_PATH_LENGTH);
        snprintf(mmdb_name, FILE_PATH_LENGTH-1, "%s/mmpo.db.%010d",conn_global->mmap_path, id);
        DEBUG("unmmapdb: %s",mmdb_name); 
        unmmap ( mmdb_name );
    }
    last_mmpo_id = id;

}		/* -----  end of function unmmapdb  ----- */

 /* vim: set ts=4 sw=4: */
