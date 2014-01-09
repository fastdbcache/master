/*
 * =====================================================================================
 *
 *       Filename:  dep_lead.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/04/2014 06:02:17 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), vyouzhi@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "dep_struct.h"
#include "dep_mem.h"


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  leadinit
 *  Description:  
 * =====================================================================================
 */
void leadinit ( size_t byte ){
    if(byte < LIMIT_SLAB_BYTE) {
        DEBUG("deposit is very little");
        exit(1);
    }
    switch (conn_global->deptype  ) {
        case D_MEM:	
            pools_dest = mem_init ( byte );
            break;

        case D_MMAP:	
            break;

        case D_HD:	
            break;

        default:	
            break;
    }				/* -----  end switch  ----- */

    if(!pools_dest){
        DEBUG("init pools_dest error");
        exit(1);
    }
}		/* -----  end of function leadinit  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  leadadd
 *  Description:  
 * =====================================================================================
 */
void leadadd ( ub1 *key, ub4 keyl ){
        
    switch ( conn_global->deptype ) {
        case D_MEM:	
            mem_set ( key, keyl );
            break;

        case D_MMAP:	
            break;

        case D_HD:	
            break;

        default:	
            break;
    }				/* -----  end switch  ----- */
}		/* -----  end of function leadadd  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  leadpush
 *  Description:  
 * =====================================================================================
 */
int leadpush ( DBP *_dbp ){
    int i;
    i = -1;

    switch ( conn_global->deptype ) {
        case D_MEM:	
            i = mem_pushdb ( _dbp );
            break;

        case D_MMAP:	
            break;

        case D_HD:	
            break;

        default:	
            break;
    }				/* -----  end switch  ----- */
    return i;
}		/* -----  end of function leadpush  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  leadexit
 *  Description:  
 * =====================================================================================
 */
int leadexit ( DBP *_dbp ){    
    ssize_t len = sizeof(char)+sizeof(uint32);
    int blen;

    if(!_dbp) return -1;

    _dbp->inBuf  = (char *)calloc(len, sizeof(char));
    if(!_dbp->inBuf) return -1;
    _dbp->inEnd = len;
    memcpy(_dbp->inBuf, "X", sizeof(char));
    blen = htonl(sizeof(uint32));
    memcpy(_dbp->inBuf+sizeof(char), &blen, sizeof(uint32));

    return 0;
}		/* -----  end of function leadexit  ----- */



 /* vim: set ts=4 sw=4: */


