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

#include "../config_global.h"
#include "../parser/Expression.h"
#include "../time_lib.h"
#include "../misc.h"

#include "lookupa.h"
#include "openmd5.h"

static const char *HashTable_for_list[] = {
    "htab.db.meta",
    "harug.db.meta",
    "hitem.row.meta",
    "tlist.db.meta",
    "hslab.db.meta",
    "fslab.db.meta",
    NULL
};

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

struct __slabpack {
    char *pack;
    int  len;
};				/* ----------  end of struct __slabpack  ---------- */

typedef struct __slabpack SLABPACK;

struct __hslab
{
  ub1           *sm;      /* slab malloc 1M */
  ub4           ss;      /* start of pool malloc */
  ub4           sf;      /* free of pool malloc if(sf==0) check fslab if return == -1 mount next*/
  /*sb2           id;        slab id */
  /*struct __hslab *next;      next */
};
typedef  struct __hslab  HSLAB;

/* record the unlink slab */
struct __fslab
{
  ssize_t           psize;     /* hpool size */
  sb2           sid;       /* slab id */
  sb2           sa;       /* data row start addr of hslab  sa*psize  sa = ss */   
  /*struct __fslab *next;      next */
};
typedef  struct __fslab  FSLAB;

/*
HSLAB  HPOOL[MAX__POOL];
dont del hitem only set drl is 0 
*/
struct __hitem
{
  ub1          key[KEY_LENGTH];      /* key that is hashed */
  ub4           keyl;     /* length of key */
  ub4           drl;      /* length of data row */
  ssize_t           psize;    /* hpools size */
  sb2           sid;      /* slab id */
  ub4           sa;       /* data row start addr of hslab  sa*psize  sa = ss */
  uint64_t          hval;     /* hash value for key */
  uint64_t           hjval;     /* hash value for key */
  ub4           utime;    /* */
  ub4           offtime;  /* offset update time default 0 */
  ub4           ahit;     /* all hit */
  ub4           amiss;    /* all update */
  struct __hitem *next;     /* next hitem in list */
};
typedef  struct __hitem  HITEM;

struct __hitem_meta
{
    
};
typedef  struct __hitem_meta HMETA;

struct __hitem_group
{
    HITEM        **usable;
    HITEM        **move;
    sb2  id;        /* which bucket */
    ub4  buckets;  /* init bucket default 1024 << 2 */
    ub4          bucket;
};
typedef  struct __hitem_group  HG;

/* algorithm Recently Used 
 * array use this struct 
 * if(htab->step < htab->logsize) add
 * else{
 *  if(insert ) LRU 
 *  if(update) MRU 
 * }
 * */
struct __haru
{
  HITEM         *phitem;  /* point to hitem */
  ub1           hid;     /* mmap hitem key id */
  ub1           id;     /* mmap hitem id */
  ub4           hit;    /* haru hit */
};
typedef  struct __haru  HARU;

struct __haru_group
{
  HARU haru_pool[MAX_HARU_POOL];    /* haru  */
  ssize_t       step;
  ssize_t       max;
  ssize_t       mix;
};
typedef  struct __haru_group  HARUG;

/* for select any proc */
struct __hdr
{
  ub1           *key;     /* sql that is hashed */
  ub4           keyl;     /* length of key */
  ub4           stime;    /* select time */
  H_STATE       flag;     /* TRUE default FALSE is del */
  ub1           *dr;      /* db return data row */
  ub4           drl;      /* length of data row */
  struct __hdr   *next;  
};
typedef  struct __hdr  HDR;

/* https://github.com/troydhanson/uthash */
struct __htab
{
  ub4           logsize; /* log of size of table */
  size_t         mask;    /* (hashval & mask) is position in table */
  ub4            count;   /* how many items in this hash table so far? 记录目前使用多少hitem_pool */
  sb2            bcount;  /* single items length 记录最长的hitem*/
  sb2            lcount;  /* single items length 记录最短的hitem*/
  ub4            hit;     /* hval hit times */
  ub4            miss;    /* hval miss times */
  ub4            set;     /* total set */
  ub4            get;     /* total get */
  ub4            bytes;    /* total data size */
  sb2            hslab_stat[MAX_SLAB_CLASS];  /* hsalb pool length */
};
typedef  struct __htab  HTAB;

struct __tlist{
  char      key[KEY_LENGTH];  /* key is table name or ulist  key sql */
  ub4       keyl;      /* length name */
  ub4       utime;  /* table the last update time */
  H_STATE       flag; /* TRUE default FALSE is del */
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
ub4 *pools_hitem_row;

pthread_mutex_t work_lock_hit;
pthread_mutex_t work_lock_miss;
pthread_mutex_t work_lock_bytes;
pthread_mutex_t work_lock_tlist;
pthread_mutex_t work_lock_hdr;

HG *hitem_group;
/* HITEM **pools_hitem;*/
HDR *pools_hdr_tail;
HDR *pools_hdr_head;
TLIST *pools_tlist;
HSLAB *pools_hslab;

pthread_mutex_t work_lock_fslab;
FSLAB *pools_fslab;

HSMS slabclass[MAX_SLAB_CLASS];

HARUG *pools_harug;
HARU *pools_haru_pool;

#define HIT_LOCK() do{\
    pthread_mutex_lock(&work_lock_hit); \
}while(0)

#define HIT_UNLOCK() do{\
    pthread_mutex_unlock(&work_lock_hit); \
}while(0)

#define MISS_LOCK() do{\
    pthread_mutex_lock(&work_lock_miss); \
}while(0)

#define MISS_UNLOCK() do{\
    pthread_mutex_unlock(&work_lock_miss); \
}while(0)

#define BYTES_LOCK() do{\
    pthread_mutex_lock(&work_lock_bytes); \
}while(0)

#define BYTES_UNLOCK() do{\
    pthread_mutex_unlock(&work_lock_bytes); \
}while(0)

#define ULIST_LOCK() do{\
    pthread_mutex_lock(&work_lock_ulist); \
}while(0)

#define ULIST_UNLOCK() do{\
    pthread_mutex_unlock(&work_lock_ulist); \
}while(0)

#define TLIST_LOCK() do{\
    pthread_mutex_lock(&work_lock_tlist); \
}while(0)

#define TLIST_UNLOCK() do{\
    pthread_mutex_unlock(&work_lock_tlist); \
}while(0)

#define HDR_LOCK() do{\
    pthread_mutex_lock(&work_lock_hdr); \
}while(0)

#define HDR_UNLOCK() do{\
    pthread_mutex_unlock(&work_lock_hdr); \
}while(0)

#define HITEM_SWITCH(y) do{\
    if(hitem_group->bucket < (y)){            \
        pools_hitem = hitem_group->move; \
    }else                               \
        pools_hitem = hitem_group->usable; \
}while(0)

#ifdef __cplusplus
 }
#endif
#endif /* --- #POOL_STRUCT_H ---*/
 /* vim: set ts=4 sw=4: */

