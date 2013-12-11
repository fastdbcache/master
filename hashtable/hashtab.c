/*
 * =====================================================================================
 *
 *       Filename:  hashtab.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/25/2013 06:00:08 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef STANDARD
#include "standard.h"
#endif
#ifndef LOOKUPA
#include "lookupa.h"
#endif
#ifndef HASHTAB
#include "hashtab.h"
#endif
#ifndef RECYCLE
#include "recycle.h"
#endif

/* sanity check -- make sure ipos, apos, and count make sense */
static void  hsanity(t)
htab *t;
{
  ub4    i, end, counter;
  hitem *h;

  /* test that apos makes sense */
  end = (ub4)1<<(t->logsize);
  if (end < t->apos)
    printf("error:  end %ld  apos %ld\n", end, t->apos);

  /* test that ipos is in bucket apos */
  if (t->ipos)
  {
    for (h=t->table[t->apos];  h && h != t->ipos;  h = h->next)
      ;
    if (h != t->ipos)
      printf("error:ipos not in apos, apos is %ld\n", t->apos);
  }

  /* test that t->count is the number of elements in the table */
  counter=0;
  for (counter=0, i=0;  i<end;  ++i)
    for (h=t->table[i];  h;  h=h->next)
      ++counter;
  if (counter != t->count)
    printf("error: counter %ld  t->count %ld\n", counter, t->count);
}


/*
 * hgrow - Double the size of a hash table.
 * Allocate a new, 2x bigger array,
 * move everything from the old array to the new array,
 * then free the old array.
 */
static void hgrow()
{
    ub4     newsize = (ub4)1<<(++pools_htab->logsize);
    ub4     newmask = newsize-1;
    ub4     i;
    HITEM   *old_hitem = pools_hitem, *new_hitem;
 
    pools_hitem = inithitem( newsize );
     
    pools_htab->logsize = newsize; 
    pools_htab->mask = newmask;
    
    new_hitem = pools_hitem;

    /* Walk through old table putting entries in new table */
    for (i=newsize>>1; i--;)
    {
        HITEM *this, *that, **newplace;
        for (this = old_hitem[i]; this;)
        {
            that = this;
            this = this->next;
            newplace = &new_hitem[(that->hval & newmask)];
            that->next = *newplace;
            *newplace = that;
        }
    }

  /* position the hash table on some existing item 
  hfirst(t);*/

  /* free the old array */
  free(old_hitem);

}

/* hdestroy - destroy the hash table and free all its memory */
void hdestroy( t)
htab  *t;    /* the table */
{
  hitem *h;
  refree(t->space);
  free((char *)t->table);
  free((char *)t);
}

/* hcount() is a macro, see hashtab.h */
/* hkey() is a macro, see hashtab.h */
/* hkeyl() is a macro, see hashtab.h */
/* hstuff() is a macro, see hashtab.h */




/* hdel - delete the item at the current position */
word  hdel(t)
htab *t;      /* the hash table */
{
  hitem  *h;    /* item being deleted */
  hitem **ip;   /* a counter */

  /* check for item not existing */
  if (!(h = t->ipos)) return FALSE;

  /* remove item from its list */
  for (ip = &t->table[t->apos]; *ip != h; ip = &(*ip)->next)
    ;
  *ip = (*ip)->next;
  --(t->count);

  /* adjust position to something that exists */
  if (!(t->ipos = h->next)) hnbucket(t);

  /* recycle the deleted hitem node */
  redel(t->space, h);

#ifdef HSANITY
  hsanity(t);
#endif  /* HSANITY */

  return TRUE;
}

/* hfirst - position on the first element in the table */
word hfirst(t)
htab  *t;    /* the hash table */
{
  t->apos = t->mask;
  (void)hnbucket(t);
  return (t->ipos != (hitem *)0);
}

/* hnext() is a macro, see hashtab.h */

/*
 * hnbucket - Move position to the first item in the next bucket.
 * Return TRUE if we did not wrap around to the beginning of the table
 */
word hnbucket(t)
htab *t;
{
  ub4  oldapos = t->apos;
  ub4  end = (ub4)1<<(t->logsize);
  ub4  i;

  /* see if the element can be found without wrapping around */
  for (i=oldapos+1; i<end; ++i)
  {
    if (t->table[i&t->mask])
    {
      t->apos = i;
      t->ipos = t->table[i];
      return TRUE;
    }
  }

  /* must have to wrap around to find the last element */
  for (i=0; i<=oldapos; ++i)
  {
    if (t->table[i])
    {
      t->apos = i;
      t->ipos = t->table[i];
      return FALSE;
    }
  }

  return FALSE;
}

void hstat(t)
htab  *t;
{
  ub4     i,j;
  double  total = 0.0;
  hitem  *h;
  hitem  *walk, *walk2, *stat = (hitem *)0;

  /* in stat, keyl will store length of list, hval the number of buckets */
  for (i=0; i<=t->mask; ++i)
  {
    for (h=t->table[i], j=0; h; ++j, h=h->next)
      ;
    for (walk=stat; walk && (walk->keyl != j); walk=walk->next)
      ;
    if (walk)
    {
      ++(walk->hval);
    }
    else
    {
      walk = (hitem *)renew(t->space);
      walk->keyl = j;
      walk->hval = 1;
      if (!stat || stat->keyl > j) {walk->next=stat; stat=walk;}
      else
      {
        for (walk2=stat;
             walk2->next && (walk2->next->keyl<j);
             walk2=walk2->next)
          ;
        walk->next = walk2->next;
        walk2->next = walk;
      }
    }
  }

  /* figure out average list length for existing elements */
  for (walk=stat; walk; walk=walk->next)
  {
    total+=(double)walk->hval*(double)walk->keyl*(double)walk->keyl;
  }
  if (t->count) total /= (double)t->count;
  else          total  = (double)0;

  /* print statistics */
  printf("\n");
  for (walk=stat; walk; walk=walk->next)
  {
    printf("items %ld:  %ld buckets\n", walk->keyl, walk->hval);
  }
  printf("\nbuckets: %ld  items: %ld  existing: %g\n\n",
         ((ub4)1<<t->logsize), t->count, total);

  /* clean up */
  while (stat)
  {
    walk = stat->next;
    redel(t->space, stat);
    stat = walk;
  }
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  findhslab
 *  Description:  
 * =====================================================================================
 */
HSLAB *findhslab ( ssize_t i, sb2 _sid){
    HITEM *hp = hi;
    HSLAB *slab, *p;
    char *res;

    slab = pools_hslab[i];

    for(p=slab; p; p=p->next){
        if(p->id == _sid){             
            return p;
        }
    }

    return NULL;
}		/* -----  end of function findhslab  ----- */





/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hsms
 *  Description:  
 * =====================================================================================
 */
int hsms ( ub4 bytes ){
    int i;
    float l;

    if(bytes > MAX_SLAB_BYTE) return -1;

    i = 0;
    l = (bytes / SLAB_BEGIN);
    while( l >= conn_global->factor ){
        l = l / conn_global->factor;
        i++;
    }
    if( i>0 ) --i;

    for ( ; slabclass[i].chunk > 0; i++ ) {
        if(slabclass[i].size >= bytes) return i;
    }

    return -1;
}		/* -----  end of function hsms  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  addfslab
 *  Description:  
 * =====================================================================================
 */
static void addfslab ( sb2 _psize, sb2 _sid, sb2 _sa ){
    FSLAB  *f, *fslab;
        
    pthread_mutex_lock(&work_lock_fslab);
    f = pools_fslab;

    while ( f && f->next ) {
        f = f->next;
    }

    fslab = (FSLAB *)calloc(1, sizeof(FSLAB));
    fslab->psize = _psize;
    fslab->sid = _sid;
    fslab->sa = _sa;
    fslab->next = NULL;
    f->next = fslab;
    pthread_mutex_unlock(&work_lock_fslab);
    return;
}		/* -----  end of static function addfslab  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  findfslab
 *  Description:  
 * =====================================================================================
 */
FSLAB *findfslab ( sb2 _psize ){
    FSLAB *fslab, *res;

    fslab = pools_fslab;

    do {
        res = fslab;
        fslab = fslab->ext;
        if(fslab == NULL) return NULL;
        if(fslab->psize == _psize){
            res->next = fslab->next;
            return fslab;
        }
    } while ( 1 );				/* -----  end do-while  ----- */
    
    return NULL;
}		/* -----  end of function findfslab  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  findslab
 *  Description:  
 * =====================================================================================
 */
FSLAB *findslab ( sb2 _psize ,int i){
    FSLAB *fslab, *fs_tmp;
    HSLAB *hslab, *hs_tmp;

    fslab = findfslab(_psize);
    
    if(fslab != NULL){
        /* 1. find a slab from freeslab */
        return fslab;
    }else{
        /* 2. find a slab from pools_hslab */
        hslab = pools_hslab[i];
        
        loop:
        
            if(hslab->sm == NULL){
                hslab->sm = (char *)calloc(MAX_SLAB_BYTE, sizeof(char));
                hslab->ss = 0;
                hslab->sf = slabclass[i].chunk;
                
            }

            if(hslab->sf > 0){
                fs_tmp = (FSLAB *)calloc(1, sizeof(FSLAB));

                memcpy(hslab->sm + hslab->ss*ph->psize, hdr->dr, hdr->drl);
                fs_tmp->sid = hslab->id;
                fs_tmp->sa = fslab->ss;
                fs_tmp->psize = _psize;
                hslab->ss++;
                hslab->sf--;

                return fs_tmp;
            }else{
                hs_tmp = hslabcreate(slabclass[i].chunk);
                hs_tmp->id = hslab->id + 1;
                hslab->next = hs_tmp;
                hslab = hslab->next;

                pools_htab->hslab_stat[i]++;
                goto loop;
            }
    }
    return NULL;
}		/* -----  end of function findslab  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hslabcreate
 *  Description:  
 * =====================================================================================
 */
HSLAB *hslabcreate ( ssize_t chunk ){
    HSLAB *h;
    h = (HSLAB *)calloc(1, sizeof(HSLAB));
    h->sm = (char *)calloc(MAX_SLAB_BYTE, sizeof(char));;
    h->ss = 0;
    h->sf = chunk;
    h->id = 0;
    h->next = NULL;
    
    return h;
}		/* -----  end of function hslabcreate  ----- */

 /* vim: set ts=4 sw=4: */

