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

    cache_shmid = shmget(IPC_PRIVATE, shms*sizeof(CQI), SHM_R | SHM_W);
    if(cache_shmid == NULL) exit(-1);

    token_efd = eventfd(0, 0);
    if(token_efd == -1) exit(1);

    cache_hash_table = hcreate(8);                      /* create hash table */

    /* Listen for notifications from other threads */
    event_set(&token_process->notify_event, token_efd,
              EV_READ | EV_PERSIST, cached_accept, token_process);
    event_base_set(token_process->base, &token_process->notify_event);
    if (event_add(&token_process->notify_event, 0) == -1) {
        d_log("Can't monitor libevent notify pipe");
        exit(1);
    }   

    event_base_loop(token_process->base, 0);

}		/* -----  end of function cached_init  ----- */



/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  cached_accept
 *  Description:  
 * =====================================================================================
 */
void cached_accept (int fd, short ev, void *arg){
    return <+return_value+>;
}		/* -----  end of function cached_accept  ----- */
 /* vim: set ts=4 sw=4: */

