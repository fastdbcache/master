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
#ifndef DEP_STRUCT_H
#define DEP_STRUCT_H
#ifdef __cplusplus
 extern "C"
 {
#endif

#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/mman.h>

#include "../config_global.h"
#include "../parser/Expression.h"
#include "../time_lib.h"

struct __deposit
{    
    H_STATE       flag;     /*  TRUE is busy,  FALSE is empty */
    ub1           *sm;      /*   malloc 1M only for insert update delete  */
    ub4           ss;      /*  start of pool malloc */
    ub4           sp;      /*  now is read point  */
    ub4           se;      /*  point to the end*/ 
};
typedef  struct __deposit  DEPO;

struct __mmposit
{
    uint32      total;   /* mmap for sa start mmap.meta.sa */
    uint32      id;  /* mmap for na start mmap.meta.na */
    uint32      offset;   /* mmap for sa data start mmap.db.0002 128*LIMIT_MMAP_BYTE */
    uint32      uuid;   /* mmap fro na data start mmap.db.0001 128*LIMIT_MMAP_BYTE */ 
   /*  void        *mmdb_sa;   mmap for sa data start mmap.db.0002 128*LIMIT_MMAP_BYTE 
    void        *mmdb_na;    mmap fro na data start mmap.db.0001 128*LIMIT_MMAP_BYTE */
};
typedef struct __mmposit MMPO;

struct __depstat
{ 
    ub4         maxbyte;  /* max  byte for deposit  */
    H_STATE     isfull;   /* default H_FALSE , H_TRUE is full */
    H_STATE     ispush;    /* push add is doing work */
    int         total;    /* total >= count  */
    int         count;   /* how much malloc depo sm */
    int         sd;     /* now start DEPO */
    int         nd;    /* now doing DEPO */
    MMPO        *pool_mmpo;  /* for mmap 0 is sa, 1 is na */
    H_USESTAT   fe;     /* the first deposit is free default H_USE , H_FREE is free */
    H_STATE doing;      /* H_TRUE one thread to do, H_FALSE none to do */
    DEPO        **pool_depo;
    DBP         *dep_dbp;
};
typedef  struct __depstat  DEST;

DEST *pools_dest;
char *pools_mmap[2];
pthread_mutex_t work_lock_depo;
pthread_mutex_t work_lock_deps_do;

#define DEPO_LOCK() do{\
    pthread_mutex_lock(&work_lock_depo); \
}while(0)

#define DEPO_UNLOCK() do{\
    pthread_mutex_unlock(&work_lock_depo); \
}while(0)

#define DEP_DO_LOCK() do{\
    pthread_mutex_lock(&work_lock_deps_do); \
}while(0)

#define DEP_DO_UNLOCK() do{\
    pthread_mutex_unlock(&work_lock_deps_do); \
}while(0)

void leadinit ( size_t byte );
int leadadd ( ub1 *key, ub4 keyl );
int leadpush ( DBP *_dbp );
DEPO *deposit_init (  );
int leadexit ( int bfd );

#ifdef __cplusplus
 }
#endif
#endif /* --- #DEP_STRUCT_H ---*/
 /* vim: set ts=4 sw=4: */
