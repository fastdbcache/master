/*
 * =====================================================================================
 *
 *       Filename:  dep_misc.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年01月05日 02时02分53秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), vyouzhi@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "dep_misc.h"

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  deposit_init
 *  Description:  
 * =====================================================================================
 */
DEPO *deposit_init (  ){
    DEPO *_depo;
    _depo = calloc(1, sizeof(DEPO));
    _depo->sm = calloc(LIMIT_SLAB_BYTE, sizeof(char));
    _depo->ss = 0;
    _depo->sp = 0;
    _depo->se = 0;
    return _depo;
}		/* -----  end of function deposit_init  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  findTabName
 *  Description:  
 *  https://github.com/GrooveStomp/Flex-Bison/tree/master/samples/sql
 * =====================================================================================
 */
void findTabName ( char *pack, ssize_t start, ssize_t end ){
    uint32 _len;
    char keyword[] = {"insert","update","delete","only","into","from"};

     memcpy(&_len, pack+sizeof(char), sizeof(uint32));
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

    return <+return_value+>;
}		/* -----  end of function findTabName  ----- */

 /* vim: set ts=4 sw=4: */


