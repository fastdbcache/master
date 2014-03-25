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
#include "dep_struct.h"
#include "dep_mem.h"
#include "dep_mmap.h"

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  leadinit
 *  Description:  
 * =====================================================================================
 */
void leadinit ( size_t byte ){
    pthread_mutex_init(&work_lock_depo, NULL);
    pthread_mutex_init(&work_lock_deps_do, NULL);

    if(byte < LIMIT_SLAB_BYTE) {
        FLOG_ERR("deposit is very little");
        exit(1);
    }
    switch (conn_global->deptype  ) {
        case D_MEM:	
            pools_dest = mem_init ( byte );
            break;

        case D_MMAP:
            pools_dest = mmap_init(byte);	
            break;

        case D_HD:	
            break;

        default:	
            break;
    }				/* -----  end switch  ----- */

    if(!pools_dest){
        FLOG_ERR("init pools_dest error");
        exit(1);
    }
}		/* -----  end of function leadinit  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  leadadd
 *  Description:  
 * =====================================================================================
 */
int leadadd ( ub1 *key, ub4 keyl ){
    int i;
    i = -1;
    switch ( conn_global->deptype ) {
        case D_MEM:	
            i = mem_set ( key, keyl );
            break;

        case D_MMAP:	
            i = mmap_set ( key, keyl );
            break;

        case D_HD:	
            break;

        default:	
            break;
    }				/* -----  end switch  ----- */
    return i;
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
            i = mmap_pushdb ( _dbp );
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
int leadexit ( int bfd ){    
    int blen;
    char len[4];
    
    Socket_Send(bfd, "X", sizeof(char));
    
    blen = htonl(sizeof(uint32));
    memcpy(len, &blen, sizeof(uint32));

    Socket_Send(bfd, len, sizeof(uint32));
    return 0;
}		/* -----  end of function leadexit  ----- */

 /* vim: set ts=4 sw=4: */
