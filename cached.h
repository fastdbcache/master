/*
 * =====================================================================================
 *
 *       Filename:  cached.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/22/2013 05:16:19 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef CACHED_H
#define CACHED_H
#ifdef __cplusplus
 extern "C"
 {
#endif

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#include "config_global.h"


typedef struct cache_query_info CQI;
struct cache_query_info{
    ub1 key;
    ub4 val;
    ssize_t no;
};

int cache_token_efd;
int cache_shmid;

void cached_init();
void cached_accept (int fd, short ev, void *arg);
#ifdef __cplusplus
 }
#endif
#endif /* --- #CACHED_H ---*/
 /* vim: set ts=4 sw=4: */

