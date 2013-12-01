/*
 * =====================================================================================
 *
 *       Filename:  cached.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/22/2013 05:16:45 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include "cached.h"


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  cached_init
 *  Description:  
 * =====================================================================================
 */
void cached_init (int shms){

    //cache_shmid = shmget(IPC_PRIVATE, shms*sizeof(CQI), SHM_R | SHM_W);
    //if(cache_shmid == -1) exit(-1);

    
}		/* -----  end of function cached_init  ----- */



/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  cached_accept
 *  Description:  
 * =====================================================================================
 */
void cached_accept (int fd, short ev, void *arg){
  //  return <+return_value+>;
}		/* -----  end of function cached_accept  ----- */
 /* vim: set ts=4 sw=4: */

