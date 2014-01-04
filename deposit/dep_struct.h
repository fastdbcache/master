/*
 * =====================================================================================
 *
 *       Filename:  dep_struct.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/04/2014 06:00:37 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), vyouzhi@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef DEP_STRUCT_H
#define DEP_STRUCT_H
#ifdef __cplusplus
 extern "C"
 {
#endif

#include <stdint.h>
#include <pthread.h>

#include "../config_global.h"

struct __depstat
{ 
    ub4         maxbyte;  /* max  byte for deposit  */
    
};
typedef  struct __depstat  DEST;

struct __deposit
{
    ub1           *de;      /*  change sql only for insert update delete  */
    ub4           del;      /*  sql length */
    H_STATE       flag;     /*  TRUE default FALSE is del */
    struct __deposit   *next;
};
typedef  struct __deposit  DEPO;

pthread_mutex_t work_lock_depo;

DEPO *pools_depo_tail;
DEPO *pools_depo_head;

#define DEPO_LOCK() do{\
    pthread_mutex_lock(&work_lock_depo); \
}while(0)

#define DEOP_UNLOCK() do{\
    pthread_mutex_unlock(&work_lock_depo); \
}while(0)

#ifdef __cplusplus
 }
#endif
#endif /* --- #DEP_STRUCT_H ---*/
 /* vim: set ts=4 sw=4: */


