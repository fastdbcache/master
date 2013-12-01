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
static void hgrow( t)
htab  *t;    /* table */
{
  register ub4     newsize = (ub4)1<<(++t->logsize);
  register ub4     newmask = newsize-1;
  register ub4     i;
  register hitem **oldtab = t->table;
  register hitem **newtab = (hitem **)malloc(newsize*sizeof(hitem *));

  /* make sure newtab is cleared */
  for (i=0; i<newsize; ++i) newtab[i] = (hitem *)0;
  t->table = newtab;
  t->mask = newmask;

  /* Walk through old table putting entries in new table */
  for (i=newsize>>1; i--;)
  {
    register hitem *this, *that, **newplace;
    for (this = oldtab[i]; this;)
    {
      that = this;
      this = this->next;
      newplace = &newtab[(that->hval & newmask)];
      that->next = *newplace;
      *newplace = that;
    }
  }

  /* position the hash table on some existing item */
  hfirst(t);

  /* free the old array */
  free((char *)oldtab);

}
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hinit
 *  Description:  
 * =====================================================================================
 */
void hinit ( work isize ){
HTAB mem_htab;   /* stat record */
int mem_hslab_stat[MAX_SLAB];
ub4 mem_hitem_row[MAX_HITEM_LENGTH_8];
HARU *mem_haru_POOL[MAX_HITEM_LENGTH];    /* haru  */
HITEM *pools_hitem;
HDR *pools_hdr;
TLIST *pools_tlist;
TLIST *pools_utist;
HSLAB *pools_hsalb;
FSLAB *pools_fslab;

    int i;

    *mem_htab = (HTAB *)calloc(1, sizeof(HTAB));
    if(mem_htab == NULL){
        perror("mem_htab calloc error");
        exit(1);
    }
    mem_htab->mask = MAX_HITEM_LENGTH_8 - 1;
    mem_htab->logsize = MAX_HITEM_LENGTH_8;
    mem_htab->count = 0;
    mem_htab->bcount = 0;
    mem_htab->lcount = 0;
    mem_htab->hit = 0;
    mem_htab->miss = 0;
    mem_htab->set = 0;
    mem_htab->get = 0;
    mem_htab->bytes = 0;
    bzero(mem_htab->hslab_stat, 0, sizeof(mem_htab->hslab_stat));

    for(i=0; i < MAX_HARU_POOL; i++){
        mem_haru_POOL[i] = (HARU *)calloc(1, sizeof(HARU));
        if(mem_haru_POOL[i] == NULL){
            perror("mem_haru_POOL callo error ");
            exit(1);
        }
    
    }
    len = ((ub4)1<<isize);
    pools_hitem = (HITEM **)malloc(sizeof(HITEM *)*(ub4)len);

    for (i=0; i<(ub4)len; ++i) pools_hitem[i] = (HITEM *)0;
    
    
    if ((semid_hdr_pool = shmget(IPC_PRIVATE, sizeof(HDR)*conn_global->process_num, IPC_CREAT | 0666)) < 0) {
        perror("shmget semid_hslab_pool");
        exit(1);
    };

    if ((semid_tlist = shmget(IPC_PRIVATE, sizeof(TLIST), IPC_CREAT | 0666)) < 0) {
        perror("shmget semid_hslab_pool");
        exit(1);
    };

    if ((semid_ulist = shmget(IPC_PRIVATE, sizeof(TLIST), IPC_CREAT | 0666)) < 0) {
        perror("shmget semid_hslab_pool");
        exit(1);
    };

    int max_slab = hslabclass();
    if(max_slab > 0){
        if ((semid_hslab_pool = shmget(IPC_PRIVATE, sizeof(HSLAB)*max_slab, IPC_CREAT | 0666)) < 0) {
            perror("shmget semid_hslab_pool");
            exit(1);
        };
    }

    if ((semid_fslab = shmget(IPC_PRIVATE, sizeof(FSLAB), IPC_CREAT | 0666)) < 0) {
        perror("shmget semid_hslab_pool");
        exit(1);
    };
    
    return ;
}		/* -----  end of function hinit  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hcreate
 *  Description:  
 * =====================================================================================
 */
void hcreate ( work isize ){    
    HTAB *htab;
    HITEM **table;

    len = ((ub4)1<<isize);
    pools_hitem = (HITEM **)malloc(sizeof(HITEM *)*(ub4)len);

    for (i=0; i<len; ++i) table[i] = (HITEM *)0;
    
    mem_htab 
    htab->logsize = isize;
    htab->mask = len-1;
    htab->count = 0;
    htab->apos = (ub4)0;
    htab->bcount = 0;

}		/* -----  end of function hcreate  ----- */

/* hcreate - create a hash table initially of size power(2,logsize) */
htab *hcreate(logsize)
word  logsize;    /* log base 2 of the size of the hash table */
{
  ub4 i,len;
  htab *t = (htab *)malloc(sizeof(htab));

  len = ((ub4)1<<logsize);
  t->table = (hitem **)malloc(sizeof(hitem *)*(ub4)len);
  for (i=0; i<len; ++i) t->table[i] = (hitem *)0;
  t->logsize = logsize;
  t->mask = len-1;
  t->count = 0;
  t->apos = (ub4)0;
  t->ipos = (hitem *)0;
  t->space = remkroot(sizeof(hitem));
  t->bcount = 0;
  return t;
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


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  haddItem
 *  Description:  
 * =====================================================================================
 */
word haddItem ( HDR *hdr ){   
    HTAB *tab;
    HITEM  *h,*hp, **hitempool;
    ub4     y, x, hjval;

    if(hdr == NULL) return;
    if(hdr->skl > LIMIT_SLAB_BYTE) return;

    x = lookup(hdr->sk,hdr->skl,0);
    hjval = jenkins_one_at_a_time_hash(hdr->sk, hdr->skl);
    
    tab = mem_htab;

    hitempool = (HITEM **)shmat(semid_hitem_pool, (void *)0, 0);

    /* make sure the key is not already there 
    for (h = table[(y=(x&t->mask))]; h; h = h->next)
    {*/

    h = hitempool[(y=(x&tab->mask))];    
    
    while ( h ){
        
        if ((x == h->hval) && 
            (keyl == h->keyl) && 
            memcmp(key, h->key, keyl))
        {
          t->apos = y;
          t->stuff = stuff;
          return TRUE;
        }
        h = h->next;
        if(!h->next)break;
    }
    
    if(!h){        
        h = malloc(sizeof(HITEM));
        h->key   = key;
        h->keyl  = keyl;
        h->stuff = stuff;
        h->hval  = x;   
    }
    else if(!h->next){
        hp = malloc(sizeof(HITEM));
        hp->key   = key;
        hp->keyl  = keyl;
        hp->stuff = stuff;
        hp->hval  = x;
        h->next = hp;
    }
    
    if (++t->count > (ub4)1<<(t->logsize))
    {
        y = (x&t->mask);
    }
    


    /*}

     find space for a new item */
    h = (hitem *)renew(t->space);

    /* make the hash table bigger if it is getting full */
    if (++t->count > (ub4)1<<(t->logsize))
    {
        y = (x&t->mask);
    }

    /* add the new key to the table */
    h->key   = key;
    h->keyl  = keyl;
    h->stuff = stuff;
    h->hval  = x;
    hp = &t->table[y];
    h->next = *hp;
    *hp = h;
    t->ipos = h;
    t->apos = y;

#ifdef HSANITY
    hsanity(t);
#endif  /* HSANITY */

    return TRUE;
}		/* -----  end of function haddItem  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hfind
 *  Description: LRU
 * =====================================================================================
 */
HITEM *hfind ( ub1 *key, ub4 keyl ){
    ub4 hval,hjval, y;
    HITEM **hitem_pool;
    HITEM *hp;
    TLIST *tlist;

    hval = lookup(key, keyl, 0);
    hjval = jenkins_one_at_a_time_hash(key, keyl);
    
    tlist = (TLIST *)(shmat(semid_tlist, (void *) 0, 0));
     
    hitem_pool = (HITEM**)(shmat(semid_hitem_pool, (void *) 0, 0));
    if(!hitem_pool){
        perror("shmat hitem_pool");
    }
    hp = hitem_pool[y=(hval&mem_htab->mask)];
    if(!hp) return NULL;
     
    while ( hp ) {
        if(hval == hp->hval &&
            (keyl == hp->keyl) &&
            (hjval == hp->hjval) 
            ){
                /*  if(tlist->next == NULL) return hp;
                else if(hp->utime > tlist->){
                
                }*/
                return hp;
        }
    }
    
    return NULL;
}		/* -----  end of function hfind  ----- */

/* hfind - find an item with a given key in a hash table */
word   hfind( t, key, keyl )
htab  *t;     /* table */
ub1   *key;   /* key to find */
ub4    keyl;  /* key length */
{
  hitem *h;
  ub4    x = lookup(key,keyl,0);
  ub4    y;
  for (h = t->table[y=(x&t->mask)]; h; h = h->next)
  {
    if ((x == h->hval) && 
        (keyl == h->keyl) && 
        !memcmp(key, h->key, keyl))
    {
      t->apos = y;
      t->ipos = h;
      return TRUE;
    }
  }
  return FALSE;
}

/*
 * hadd - add an item to a hash table.
 * return FALSE if the key is already there, otherwise TRUE.
 */
word hadd( t, key, keyl, stuff)
htab  *t;      /* table */
ub1   *key;    /* key to add to hash table */
ub4    keyl;   /* key length */
void  *stuff;  /* stuff to associate with this key */
{
  register hitem  *h,**hp;
  register ub4     y, x = lookup(key,keyl,0);

  /* make sure the key is not already there */
  for (h = t->table[(y=(x&t->mask))]; h; h = h->next)
  {
    if ((x == h->hval) && 
        (keyl == h->keyl) && 
        !memcmp(key, h->key, keyl))
    {
      t->apos = y;
      t->ipos = h;
      return FALSE;
    }
  }

  /* find space for a new item */
  h = (hitem *)renew(t->space);

  /* make the hash table bigger if it is getting full */
  if (++t->count > (ub4)1<<(t->logsize))
  {
    hgrow(t);
    y = (x&t->mask);
  }

  /* add the new key to the table */
  h->key   = key;
  h->keyl  = keyl;
  h->stuff = stuff;
  h->hval  = x;
  hp = &t->table[y];
  h->next = *hp;
  *hp = h;
  t->ipos = h;
  t->apos = y;

#ifdef HSANITY
  hsanity(t);
#endif  /* HSANITY */

  return TRUE;
}

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
 *         Name:  hslab
 *  Description:  
 * =====================================================================================
 */
char *gslab ( HITEM *hi ){
    HITEM *hp = hi;
    HSLAB **hslab_pool, *slab, *p;
    int i;    
    char *res;
    **hslab_pool = (HSLAB**)(shmat(semid_hslab_pool, (void *) 0, 0));
    if(!hslab_pool){
        perror("shmat hitem_pool");
    }

    i = hsms(hi->psize);

    slab = hslab_pool[i];

    for(p=slab; p; p=p->next){
        if(p->id == p->sid){
            res = p->sm + p->id * hi->psize;  
            return res;
        }
    }

    return NULL;
}		/* -----  end of function hslab  ----- */



/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hslabclass
 *  Description:  
 * =====================================================================================
 */
int hslabclass ( void ){
    int size = SLAB_BEGIN;
    int i=0;
    
    while (i++ < MAX_SLAB_CLASS && size <= MAX_SLAB_BYTE / slabclass) {
        if (size % CHUNK_ALIGN_BYTES)
            size += CHUNK_ALIGN_BYTES - (size % CHUNK_ALIGN_BYTES);
        /*if((item_size_max / size) < 2) break; 

        printf("slab class  chunk size %9u perslab %7u\n",
                     size, (MAX_SLAB_BYTE / size));*/
        slabclass[i].size = size;
        slabclass[i].chunk = (MAX_SLAB_BYTE / size);

        size *= conn_global->factor;
    } 
    slabclass[i].size = MAX_SLAB_BYTE;
    slabclass[i].chunk = 1;

    return (++i);
}		/* -----  end of function hslabclass  ----- */


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

 /* vim: set ts=4 sw=4: */

