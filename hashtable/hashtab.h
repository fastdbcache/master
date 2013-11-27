/*
 * =====================================================================================
 *
 *       Filename:  hashtab.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/25/2013 06:00:04 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef HASHTAB_H
#define HASHTAB_H
#ifdef __cplusplus
 extern "C"
 {
#endif

#include "standard.h"

#define MAX_POOL 39
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

struct _hslab
{
  ub1           *sm;      /* slab malloc 1M */
  ub4           ss;      /* start of pool malloc */
  ub4           sf;      /* free of pool malloc if(sf==0) check fslab if return == -1 mount next*/
  sb2           id;       /* slab id */
  struct _hslab *next;     /* next */
};
typedef  struct _hslab  HSLAB;


/* record the unlink slab */
struct _fslab
{
  sb2           psize;     /* hpool size */
  sb2           sid;       /* slab id */
  sb2           sa;       /* data row start addr of hslab  sa*psize */   
  struct _fslab *next;     /* next */
};
typedef  struct _fslab  FSLAB;

HSLAB  HPOOL[MAX_POOL];

struct _hitem
{
  ub1          *key;      /* key that is hashed */
  ub4           keyl;     /* length of key */
  ub4           drl;      /* length of data row */
  sb2           psize;    /* hpools size */
  sb2           sid;      /* slab id */
  ub4           sa;       /* data row start addr of hslab  sa*psize */
  ub4           hval;     /* hash value */
  ub4           utime;    /* */
  ub4           ahit;     /* all hit */
  struct _hitem *next;     /* next hitem in list */
};
typedef  struct _hitem  HITEM;

/* algorithm Recently Used 
 * array use this struct 
 * if(htab->count < htab->logsize) MRU
 * else{
 *  if(insert ) LRU 
 *  if(update) MRU 
 * }
 * */
struct _haru
{
  ub4           hval;     /* hash value */
  ub4           hit;    /* haru hit */
};
typedef  struct _haru  HARU;

/* for select any proc */
struct _hdr
{
  ub1           *sql;     /* sql that is hashed */
  ub4           sqll;     /* length of key */
  ub4           hval;     /* hash value */
  ub4           stime;    /* select time */
  ssize_t       flag;     /* 0 is new, 1 is update */
  ub1           *dr;      /* db return data row */
  ub4           drl;      /* length of data row */
};
typedef  struct _hdr  HDR;

//https://github.com/troydhanson/uthash
struct _htab
{
  word           logsize; /* log of size of table */
  size_t         mask;    /* (hashval & mask) is position in table */
  ub4            count;   /* how many items in this hash table so far? */
  ub4            hit;     /* hval hit times */
  ub4            miss;    /* hval miss times */
  ub4            set;     /* total set */
  ub4            get;     /* total get */
  ub4            bytes;    /* total data size */
  
};
typedef  struct _htab  HTAB;

struct _tlist{
  char      *name;  /* name is table name */
  ub4       utime;  /* table the last update time */
  ssize_t   flag;   /* 1.work set 1 , 2.mem set 0, 3.work free 0 */
  struct _tlist *next;  /* the next table */
};
/* mem proc table list */
typedef struct _tlist TLIST;

/* work proc for change table list */
typedef struct _tlist ULIST;

int hslab_semid;   /* slab leave in data row of the store*/
int fslab_semid;   /* free slab */
int hitem_semid;   /* hitem struct */
int haru_semid;    /* haru  */
int hdr_semid;     /* any work proc of data row */
int htab_semid;   /* stat record */
int tlist_semid;  /* mem proc change, and work proc read table list shm id */
int ulist_semid;  /* any work proc change flag=1, and work proc read from this to tlist */

hitem *iteminit();
htab *hcreate();
void hdestroy();
word hfind();
word hadd();
word hdel();
void hstat();

#ifdef __cplusplus
 }
#endif
#endif /* --- #HASHTAB_H ---*/
 /* vim: set ts=4 sw=4: */

