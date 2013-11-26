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



struct _hitem
{
  ub1          *key;      /* key that is hashed */
  ub4           keyl;     /* length of key */
  ub4           hval;     /* hash value */
  ub4           utime;    /* */
  ub4           ahit;     /* all hit */
  struct _hitem *next;     /* next hitem in list */
};
typedef  struct _hitem  HITEM;
//https://github.com/troydhanson/uthash
struct _htab
{
  word           logsize; /* log of size of table */
  size_t         mask;    /* (hashval & mask) is position in table */
  ub4            count;   /* how many items in this hash table so far? */
  ub4            apos;    /* position in the array */
  ub4            bcount;  /* # hitems useable in current block */
  ub4            hit;     /* hval hit times */
  ub4            miss;    /* hval miss times */
};
typedef  struct _htab  HTAB;

struct _tlist{
  ub4       hval;
  char      *name;
  ub4       utime; 
};
typedef struct _tlist TLIST;

struct _ulist{
  char      *name;
  struct _ulist *next; 
};
typedef struct _ulist ULIST;

int hitem_semid;
int htab_semid;
int tlist_semid;
int ulist_semid;

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

