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
    _dest = calloc(1, sizeof(DEST));
    _dest->total = num;
    _dest->maxbyte = byte;
    _dest->count = 1;
    _dest->isfull = H_FALSE;
    _dest->sd = 0;
    _dest->nd = 0;
    _dest->fe = H_USE;
    _dest->doing = H_FALSE;
    _dest->pool_depo = (DEPO **)calloc(num, sizeof(DEPO));
    for(i=0; i< num; i++){
        _dest->pool_depo[i] = deposit_init();
    }
    
    pthread_mutex_init(&work_lock_depo, NULL);
    pthread_mutex_init(&work_lock_deps_do, NULL);
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
   
    if(!_dest)  return -1;
    if(keyl > LIMIT_SLAB_BYTE) return -1;

    _lens = keyl;
    if (_lens % CHUNK_ALIGN_BYTES)
            _lens += CHUNK_ALIGN_BYTES - (_lens % CHUNK_ALIGN_BYTES);
    /* _end = (LIMIT_SLAB_BYTE / _len); */
    DEPO_LOCK();
    _depo = _dest->pool_depo[_dest->sd];
    if(_depo->se + _lens > LIMIT_SLAB_BYTE){        

        if(_dest->sd < (_dest->total-1) &&
            (_dest->sd+1) != _dest->nd){
            _dest->sd++;
        }else if(_dest->sd == _dest->nd &&
                _depo->ss == _depo->se){
            _dest->sd = 0;
            _dest->nd = 0;
            _depo->isfull = H_FALSE;
        }else {
            _dest->isfull = H_TRUE;
            DEBUG("sd is eq total");
            return -1;
        }
        
        _depo = _dest->pool_depo[_dest->sd];
        _depo->ss=0;
        _depo->sp=0;
        _depo->se=0;
    }
    if(!_depo->sm) {
        if(_dest->count * LIMIT_SLAB_BYTE > _dest->maxbyte) {
            DEBUG("isfull is H_TRUE, can't malloc");
            return -1;    
        }
        _depo->sm = calloc(1, LIMIT_SLAB_BYTE*sizeof(char));
        if(!_depo->sm){
            DEBUG("malloc error");
            return -1;
        }
        _dest->count++;
    }
    
    memcpy(_depo->sm+_depo->se, key, keyl);
    _depo->se += _lens;
 
    DEPO_UNLOCK();

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
    uint32 _lens;
    _ly *ply;
    long utime;
        
    if(!_dbp) return -1;
     
    _depo = _dest->pool_depo[_dest->nd];
    if(!_depo){
        return -1;
    }
    
    if(_depo->sp == _depo->se &&
        _depo->ss == _depo->se){

            if(_dest->sd == _dest->nd){                
                return -1;
            }else if(_dest->nd < (_dest->total-1)){
                
                _depo->sp=0;
                _depo->ss=0;
                _depo->se=0;
                _dest->nd++;
            }else{
                _dest->nd = 0;                
            }
            _depo = _dest->pool_depo[_dest->nd];
        }
    }
    _depo->sp = _depo->se;       

    loop_depo: 

        if(!_depo->sm) return -1;
        if(_depo->ss == _depo->sp) return -1;

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
            if(_depo->ss == _depo->sp) return -1;

            goto loop_depo;
        }
        /* update tlist  */
        utime = get_sec();
        pushList((ub1 *)ply->tab, ply->len, utime);

        free(ply->tab);
        free(ply);
        _dbp->inBuf = _depo->sm+_depo->ss;
        _dbp->inEnd = _lens+sizeof(char)+sizeof(uint32);
        _depo->ss += _dbp->inEnd;

       // pools_dest->isfull = H_FALSE; 

    return 0;
}		/* -----  end of function mem_pushdb  ----- */

 /* vim: set ts=4 sw=4: */
