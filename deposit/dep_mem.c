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

#include "dep_struct.h"
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  mem_init
 *  Description:  
 * =====================================================================================
 */
DEST *mem_init ( int num, size_t byte ){
    DEST *_dest;
    _dest = calloc(1, sizeof(DEST));
    _dest->maxbyte = byte;
    _dest->count = 1;
    _dest->isfull = H_FALSE;
    _dest->sd = 0;
    _dest->nd = 0;
    _dest->fe = H_USE;
    _dest->pool_depo = (DEPO *)calloc(num, sizeof(DEPO));

    return _dest;
}		/* -----  end of function mem_init  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  mem_set
 *  Description:  
 * =====================================================================================
 */
void mem_set ( ub1 *key, ub4 keyl ){
    DEST *_dest = pools_dest; 
    DEPO *_depo;
    ub4 _end, _len;
    
    _len = keyl;
    if (_len % CHUNK_ALIGN_BYTES)
            _len += CHUNK_ALIGN_BYTES - (_len % CHUNK_ALIGN_BYTES);
    /* _end = (LIMIT_SLAB_BYTE / _len); */
    DEPO_LOCK();
    _depo = _dest->pool_depo[_dest->no];
    if(_depo->se + _len > LIMIT_SLAB_BYTE){
        if(_dest->fe == H_FREE){
            _dest->no = 0; 
        }else{
            _dest->pool_depo[_dest->no++] = deposit_init();            
            _dest->count++;
        }
        _depo = _dest->pool_depo[_dest->no];
    }
 
    memcpy(_depo->sm+_dep->se, key, keyl);
    _depo->se += _len;
    if(_dest->count * LIMIT_SLAB_BYTE >= _dest->maxbyte){
        _dest->isfull = H_TRUE;
    }
    DEOP_UNLOCK();

}		/* -----  end of function mem_set  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  mem_pushdb
 *  Description:  
 * =====================================================================================
 */
void mem_pushdb (  ){
    DEST *_dest = pools_dest; 
    DEPO *_depo;      
    ub4 _end, _point;
    ub1 *_data, *_pack;
    uint32 _len;
    char table[256];
    char keyword[] = {"insert","update","delete","only","into","from","."};
    int i, move;

    while(){
        _depo = _dest->pool_depo[_dest->sd];
                    
        if(_depo->sp == _depo->se &&
            _depo->ss == _depo->se){
            if(_dest->sd == _dest->nd)
                break;
            else
                _dest->sd++;
        }
        _depo->sp = _depo->se;       
        _point = _depo->ss;

        while(){ 
                        
            memcpy(&_len, _depo->sm+_point+sizeof(char), sizeof(uint32));
            _len = ntohl(_len);

            if(_point+sizeof(char)+_len > _depo->sp){
                break;
            }
            _point += sizeof(char)+sizeof(uint32);
            _data = _depo->sm + _point;
            _point += _len + sizeof(char);

            SPACE(_data);
            for(i=0; i<6; i++){            
                if(tolower(*_data) != keyword[0][i] ||
                    tolower(*_data) != keyword[1][i] ||
                    tolower(*_data) != keyword[2][i]
                    ){
                    goto error; 
                }
            }
            _data += 6;
            SPACE(_data);
            if(!memcmp(_data, keyword[3], 4)){
                _data+=4;
                SPACE(_data);
            }
            if(memcmp(_data, keyword[4], 4) ||
                memcmp(_data, keyword[5], 4)
                ){
                goto error;
            }
            SPACE(_data);
            move = 0;
            while(*(_data+move) != ' '){
                if(*(_data+move) == '.' ){
                    _data += move+1;
                    move = 0;
                }else
                    move ++;
            
            }
            if(move==0) goto error;
            /* update tlist  */
            pushList(_data, move, get_sec());
        }

        Socket_Send(backend, _depo->sm+_depo->ss, _depo->sp);
        if(!_pack){
            _pack = calloc(1, sizeof(char));
        }

        Socket_Read(backend,_pack, sizeof(char));
        
        error:
            break;            
    }
     
    return <+return_value+>;
}		/* -----  end of function mem_pushdb  ----- */

 /* vim: set ts=4 sw=4: */


