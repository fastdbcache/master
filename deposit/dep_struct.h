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
#include "../parser/da.h"
#include "../parser/sql1.h"
#include "../time_lib.h"
#include "../hashtable/pool_proc.h"

struct __deposit
{    
    H_STATE       flag;     /*  TRUE is busy,  FALSE is empty */
    ub1           *sm;      /*   malloc 1M only for insert update delete  */
    ub4           ss;      /*  start of pool malloc */
    ub4           sp;      /*  now is read point  */
    ub4           se;      /*  point to the end*/ 
};
typedef  struct __deposit  DEPO;

struct __depstat
{ 
    ub4         maxbyte;  /* max  byte for deposit  */
    H_STATE     isfull;   /* default H_FALSE , H_TRUE is full */
    sb2         count;   /* how much malloc depo sm */
    sb2         sd;     /* now start DEPO */
    sb2         nd;    /* now doing DEPO */
    H_USESTAT   fe;     /* the first deposit is free default H_USE , H_FREE is free */
    DEPO        **pool_depo;
};
typedef  struct __depstat  DEST;

DEST *pools_dest;

pthread_mutex_t work_lock_depo;

#define DEPO_LOCK() do{\
    pthread_mutex_lock(&work_lock_depo); \
}while(0)

#define DEOP_UNLOCK() do{\
    pthread_mutex_unlock(&work_lock_depo); \
}while(0)

void leadinit ( size_t byte );
void leadadd ( ub1 *key, ub4 keyl );
void leadpush (  );
DEPO *deposit_init (  );

#ifdef __cplusplus
 }
#endif
#endif /* --- #DEP_STRUCT_H ---*/
 /* vim: set ts=4 sw=4: */


