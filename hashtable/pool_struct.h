/*
 * =====================================================================================
 *
 *       Filename:  pool_struct.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/11/2013 10:05:26 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef POOL_STRUCT_H
#define POOL_STRUCT_H
#ifdef __cplusplus
 extern "C"
 {
#endif

#include <pthread.h>

#include "standard.h"
#include "../config_global.h"
#include "../parser/da.h"
#include "../parser/sql1.h"

#define MAX_HITEM_LENGTH 1024
#define MAX_HITEM_LENGTH_8 (MAX_HITEM_LENGTH<<8)
#define MAX_HARU_POOL 1024
#define MAX_SLAB_CLASS  200
#define CHUNK_ALIGN_BYTES 8
#define MAX_SLAB_BYTE 1024 * 1024
#define LIMIT_SLAB_BYTE 1024 * 1024
#define SLAB_BEGIN 88
/* 
 * f=1.25
 * slab class   1: chunk size     88 perslab 11915
 * slab class   2: chunk size    112 perslab  9362
 * slab class   3: chunk size    144 perslab  7281
 * slab class   4: chunk size    184 perslab  5698
 * slab class   5: chunk size    232 perslab  4519
 * slab class   6: chunk size    296 perslab  3542 
 * ...
 * slab class  37: chunk size 312976 perslab     3  
 * slab class  38: chunk size 391224 perslab     2  
 * slab class  39: chunk size 489032 perslab     2  
 * */
typedef enum {
    H_TRUE=0,  /* default 0 ,it's 1 has a job, 2 working */   
    H_FALSE
} H_STATE;

struct __hslab
{
  ub1           *sm;      /* slab malloc 1M */
  ub4           ss;      /* start of pool malloc */
  ub4           sf;      /* free of pool malloc if(sf==0) check fslab if return == -1 mount next*/
  sb2           id;       /* slab id */
  struct __hslab *next;     /* next */
};
typedef  struct __hslab  HSLAB;


/* record the unlink slab */
struct __fslab
{
  sb2           psize;     /* hpool size */
  sb2           sid;       /* slab id */
  sb2           sa;       /* data row start addr of hslab  sa*psize  sa = ss */   
  struct __fslab *next;     /* next */
};
typedef  struct __fslab  FSLAB;

//HSLAB  HPOOL[MAX__POOL];

struct __hitem
{
  ub1          *key;      /* key that is hashed */
  ub4           keyl;     /* length of key */
  ub4           drl;      /* length of data row */
  sb2           psize;    /* hpools size */
  sb2           sid;      /* slab id */
  ub4           sa;       /* data row start addr of hslab  sa*psize  sa = ss */
  ub4           hval;     /* hash value for key */
  ub4           hjval;     /* hash value for key */
  ub4           utime;    /* */
  ub4           ahit;     /* all hit */
  struct __hitem *next;     /* next hitem in list */
};
typedef  struct __hitem  HITEM;

/* algorithm Recently Used 
 * array use this struct 
 * if(htab->count < htab->logsize) MRU
 * else{
 *  if(insert ) LRU 
 *  if(update) MRU 
 * }
 * */
struct __haru
{
  ub4           hval;     /* hash value */
  ub4           hit;    /* haru hit */
};
typedef  struct __haru  HARU;

/* for select any proc */
struct __hdr
{
  ub1           *key;     /* sql that is hashed */
  ub4           keyl;     /* length of key */
  ub4           stime;    /* select time */
  H_STATE       flag;     /* 0 is new, 1 is update, HDR_FALSE worker delete, proc change */
  ub1           *dr;      /* db return data row */
  ub4           drl;      /* length of data row */
  pthread_t     pid;
  struct __hdr   *next;  
};
typedef  struct __hdr  HDR;

//https://github.com/troydhanson/uthash
struct __htab
{
  word           logsize; /* log of size of table */
  size_t         mask;    /* (hashval & mask) is position in table */
  ub4            count;   /* how many items in this hash table so far? 记录目前使用多少hitem_pool */
  sb2            bcount;  /* single items length 记录最长的hitem*/
  sb2            lcount;  /* single items length 记录最短的hitem*/
  ub4            hit;     /* hval hit times */
  ub4            miss;    /* hval miss times */
  ub4            set;     /* total set */
  ub4            get;     /* total get */
  ub4            bytes;    /* total data size */
  sb2            hslab_stat[MAX_SLAB];  /* hsalb pool length */
};
typedef  struct __htab  HTAB;

struct __tlist{
  char      *key;  /* key is table name or ulist  key sql */
  ub4       keyl      /* length name */
  ub4       utime;  /* table the last update time */
  H_STATE   flag;   /* 1.work set H_TRUE , 2.proc set H_FALSE, 3.work free H_FALSE */
  struct __tlist *next;  /* the next table */
};
/* mem proc table list */
typedef struct __tlist TLIST;

/* work proc for change table list */
typedef struct __tlist ULIST;

/* slab malloc slot 1M */
struct __hsms{
  ub4       size; 
  ssize_t   chunk;   /* 1.work set 1 , 2.mem set 0, 3.work free 0 */
};
/* mem proc table list */
typedef struct __hsms HSMS;

HTAB *pools_htab;   /* stat record */
int pools_hslab_stat[MAX_SLAB];
ub4 pools_hitem_row[MAX_HITEM_LENGTH_8];

HARU pools_haru_POOL[MAX_HARU_POOL];    /* haru  */

HITEM **pools_hitem;
HDR **pools_hdr;
TLIST *pools_tlist;
TLIST **pools_ulist;
HSLAB **pools_hslab;

pthread_mutex_t work_lock_fslab;
FSLAB *pools_fslab;

HSMS slabclass[MAX_SLAB_CLASS];

#ifdef __cplusplus
 }
#endif
#endif /* --- #POOL_STRUCT_H ---*/
 /* vim: set ts=4 sw=4: */

