/*
 * =====================================================================================
 *
 *       Filename:  pg_misc.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/06/2014 12:27:04 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "pg_misc.h"

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  initdbp
 *  Description:  
 * =====================================================================================
 */
DBP *initdbp (  ){
    DBP *_dbp;
    _dbp = (DBP *)calloc(1, sizeof(DBP));
    _dbp->inBuf = NULL;
    _dbp->inBufSize = 0;
    _dbp->inCursor = 0;
    _dbp->inEnd = 0;

    return _dbp;
}		/* -----  end of function initdbp  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  freedbp
 *  Description:  
 * =====================================================================================
 */
void freedbp ( DBP *_dbp ){
    if(!_dbp) return;
    if(_dbp->inBuf) free(_dbp->inBuf);
    free(_dbp);
}		/* -----  end of function freedbp  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  CheckBufSpace
 *  Description:  
 * =====================================================================================
 */
int CheckBufSpace ( ssize_t endPos, DBP *_dbp ){
    char *newbuf;

    if(!_dbp)return -1;
    
    if(_dbp->inEnd != 0){
        _dbp->inCursor = _dbp->inEnd;
    }

    _dbp->inEnd += endPos;
    if(_dbp->inBufSize > _dbp->inEnd) return 0;

    newbuf = (char *)realloc(_dbp->inBuf, _dbp->inEnd);
    _dbp->inBufSize = _dbp->inEnd; 
    
    if(newbuf){
        _dbp->inBuf = newbuf;
    } else{
        freedbp(_dbp); 
        return -1;
    }
    return 0;
}		/* -----  end of function CheckBufSpace  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  getInt
 *  Description:  
 * =====================================================================================
 */
int getInt ( int *result, size_t bytes, DBP *_dbp ){
    uint32 t32;
    uint16 t16;

    switch ( bytes ) {
        case 2:	
            memcpy(&t16, _dbp->inBuf + _dbp->inCursor, sizeof(uint16));
            *result = (int)ntohs(t16);
            break;

        case 4:	
            memcpy(&t32, _dbp->inBuf + _dbp->inCursor, sizeof(uint32));
            *result = (int)htonl(t32);
            break;

        default:	
            return -1;
    }				/* -----  end switch  ----- */
    return 0;
}		/* -----  end of function getInt  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  getCont
 *  Description:  
 * =====================================================================================
 */
void getCont (  ){
    return ;
}		/* -----  end of function getCont  ----- */

 /* vim: set ts=4 sw=4: */

