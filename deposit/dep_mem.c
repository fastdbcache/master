/*
 * =====================================================================================
 *
 *       Filename:  dep_mem.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/04/2014 05:59:48 PM
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
#include "dep_mem.h"
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  mem_init
 *  Description:  
 * =====================================================================================
 */
DEST *mem_init ( size_t byte ){
    DEST *_dest;
    int i, num;

    num = (int)(byte / (LIMIT_SLAB_BYTE));
    DEBUG("num:%d", num);
    _dest = calloc(1, sizeof(DEST));
    if(!_dest) return NULL;

    _dest->total = num;
    _dest->maxbyte = byte;
    _dest->count = 1;
    _dest->isfull = H_FALSE;
    _dest->ispush = H_FALSE;
    _dest->sd = 0;
    _dest->nd = 0;
    _dest->fe = H_USE;
    _dest->pool_mmpo = NULL;
    _dest->doing = H_FALSE;
    _dest->pool_depo = (DEPO **)calloc(num, sizeof(DEPO));
    if(!_dest->pool_depo) return NULL;

    for(i=0; i< num; i++){
        _dest->pool_depo[i] = deposit_init();
    }
    

    return _dest;
}		/* -----  end of function mem_init  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  mem_set
 *  Description:  
 * =====================================================================================
 */
int mem_set ( ub1 *key, ub4 keyl ){
    DEST *_dest = pools_dest; 
    DEPO *_depo;
    ub4  _lens;
    uint32 mask;
 
    if(!_dest){  
        DEBUG("pools_dest is null");
        return -1;
    }
    if(keyl > (LIMIT_SLAB_BYTE)){ 
        DEBUG("keyl is too big %d", keyl);
        return -1;
    }

    _lens = alignByte(keyl);
    mask = _dest->total-1;
    /* _end = (LIMIT_SLAB_BYTE / _len); */
    DEPO_LOCK();
    _depo = _dest->pool_depo[_dest->sd];
    if(_depo->se + _lens > (LIMIT_SLAB_BYTE)){
        if(_dest->sd < mask &&
            ((_dest->sd+1)&mask) != _dest->nd){
            _dest->sd = (_dest->sd+1)&mask;
            
        /*}else if(_dest->sd == _dest->nd &&
                _depo->ss == _depo->se){  */
        }else if(_dest->ispush == H_TRUE){
            _dest->sd = 0;
            _dest->nd = 0;
            /*_dest->isfull = H_FALSE;  */
        }else {
            DEBUG("full");
            _dest->isfull = H_TRUE;
            _dest->ispush = H_FALSE;
            DEPO_UNLOCK();
            return -1;
        }
        
        _depo = _dest->pool_depo[_dest->sd];
        _depo->ss=0;
        _depo->sp=0;
        _depo->se=0;
    }
    if(!_depo->sm) {
        _depo->ss=0;
        _depo->sp=0;
        _depo->se=0;
        if(_dest->count * (LIMIT_SLAB_BYTE) > _dest->maxbyte) {            
            DEBUG("no momey use for dep_mem");
            DEPO_UNLOCK();
            return -1;    
        }else{
            DEBUG("use mem:%llu, max:%llu", _dest->count * (LIMIT_SLAB_BYTE), _dest->maxbyte);
        }
        _depo->sm = calloc(1, LIMIT_SLAB_BYTE*sizeof(ub1));
        if(!_depo->sm){
            DEPO_UNLOCK();
            return -1;
        }
        _dest->count++;
    }
    DEBUG("sd:%d, ss:%llu", _dest->sd, _depo->ss);
    memcpy(_depo->sm+_depo->se, key, keyl);
    _depo->se += _lens;
 
    DEPO_UNLOCK();
    /*   DEBUG("nd:%d, sp:%d, ss:%d, se:%d",_dest->nd, _depo->sp, _depo->ss, _depo->se); */
    return 0;
}		/* -----  end of function mem_set  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  mem_pushdb
 *  Description:  
 * =====================================================================================
 */
int mem_pushdb ( DBP *_dbp ){
    DEST *_dest = pools_dest; 
    DEPO *_depo;      
    uint32 _lens, offset, mask;
    _ly *ply;
    long utime;
     
    DEBUG("mem_pushdb"); 
    if(!_dbp) return -1;
     
    _depo = _dest->pool_depo[_dest->nd];
    if(!_depo){
        return -1;
    }
    mask = _dest->total-1;
    /*  DEBUG("pushdb sd:%d nd:%d, sp:%d, ss:%d, se:%d",_dest->sd, _dest->nd, _depo->sp, _depo->ss, _depo->se);   */
    if(_depo->sp == _depo->se &&
        _depo->ss == _depo->se){

        if(_dest->sd == _dest->nd){
            return -1;
        }else {
            _depo->sp=0;
            _depo->ss=0;
            _depo->se=0;
            _dest->nd = ((_dest->nd+1)&mask);
        }
        _depo = _dest->pool_depo[_dest->nd];
        
    }
    _depo->sp = _depo->se;       

    loop_depo: 

        if(!_depo->sm){ 
            DEBUG("sm is null");
            return -1;
        }
        if(_depo->ss == _depo->sp){
            DEBUG("ss eq sp");
            return -1;
        }

        if(*(_depo->sm+_depo->ss) != 'Q'){
            _depo->ss++;
            goto loop_depo;
        }
         
        memcpy(&_lens, _depo->sm+_depo->ss+sizeof(char), sizeof(uint32));
        _lens = ntohl(_lens);

        if(_depo->ss+sizeof(char)+_lens > _depo->sp){
            return -1;
        }
        _lens -= sizeof(uint32);
        ply = parser_do ((char *)(_depo->sm+_depo->ss+sizeof(char)+sizeof(uint32)), _lens);

        if(!ply){ 
            /* DEBUG("ply is null %s", _u->key);*/

            _depo->ss += _lens + sizeof(char) + sizeof(uint32);
            if(_depo->ss == _depo->sp){ 
                DEBUG("ply is null");
                return -1;
            }

            goto loop_depo;
        }
        /* update tlist  */
        utime = get_sec();
        pushList((ub1 *)ply->tab, ply->len, utime);

        free(ply->tab);
        free(ply);

        _dbp->inBuf = _depo->sm+_depo->ss;
        _dbp->inEnd = _lens+sizeof(char)+sizeof(uint32);
        offset = _dbp->inEnd;
        if (offset % CHUNK_ALIGN_BYTES)
            offset += CHUNK_ALIGN_BYTES - (offset % CHUNK_ALIGN_BYTES);
        _depo->ss += offset;

       // pools_dest->isfull = H_FALSE; 

    return 0;
}		/* -----  end of function mem_pushdb  ----- */

 /* vim: set ts=4 sw=4: */
