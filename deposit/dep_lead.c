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
void leadpush (  ){

    switch ( conn_global->deptype ) {
        case D_MEM:	
            mem_pushdb (  );
            break;

        case D_MMAP:	
            break;

        case D_HD:	
            break;

        default:	
            break;
    }				/* -----  end switch  ----- */
}		/* -----  end of function leadpush  ----- */
 /* vim: set ts=4 sw=4: */


