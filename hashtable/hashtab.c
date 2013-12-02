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
    ub4     newsize = (ub4)1<<(++pools_htab->logsize);
    ub4     newmask = newsize-1;
    ub4     i;
    HITEM   *old_hitem = pools_hitem, *new_hitem;
  register hitem **newtab = (hitem **)malloc(newsize*sizeof(hitem *));
 
    pools_hitem = (HITEM *)calloc(newsize, sizeof(HITEM));
    /* pools_hitem head not store anything */
    for (i=0; i<newsize; ++i) pools_hitem[i]->next = NULL;
 
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

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hcreate
 *  Description:  
 * =====================================================================================
 */
void hcreate ( work isize ){
    int i;
    ub4 len;
    len = ((ub4)1<<isize);

    *pools_htab = (HTAB *)calloc(1, sizeof(HTAB));
    if(pools_htab == NULL){
        perror("pools_htab calloc error");
        exit(1);
    }
    pools_htab->mask = len - 1;
    pools_htab->logsize = len;
    pools_htab->count = 0;
    pools_htab->bcount = 0;
    pools_htab->lcount = 0;
    pools_htab->hit = 0;
    pools_htab->miss = 0;
    pools_htab->set = 0;
    pools_htab->get = 0;
    pools_htab->bytes = 0;
    bzero(pools_htab->hslab_stat, 0, sizeof(pools_htab->hslab_stat));

    for(i=0; i < MAX_HARU_POOL; i++){
        pools_haru_pool[i] = (HARU *)calloc(1, sizeof(HARU));
        if(pools_haru_POOL[i] == NULL){
            perror("pools_haru_POOL callo error ");
            exit(1);
        }
    
    }
    
    pools_hitem = (HITEM *)calloc((ub4)len, sizeof(HITEM));
    /* pools_hitem head not store anything */
    for (i=0; i<(ub4)len; ++i) pools_hitem[i]->next = NULL;
    
    pools_hdr = (HDR *) calloc(conn_global->process_num, sizeof(HDR));    

    pools_tlist = (TLIST *)calloc(1, sizeof(TLIST));

    pools_utist = (TLIST *)calloc(conn_global->process_num, sizeof(TLIST));

    int max_slab = hslabclass();

    if(max_slab > 0){
        pools_hslab = (HSLAB *)calloc(max_slab, sizeof(HSLAB));        
        inithslab ( max_slab );
    }

    pthread_mutex_init(&work_lock_fslab, NULL);
    pools_fslab = (FSLAB *)calloc(1, sizeof(FSLAB));
    pools_fslab->psize = 0;
    pools_fslab->sid = 0;
    pools_fslab->sa = 0;    
    pools_fslab->next = NULL;
    return ;
}		/* -----  end of function hcreate  ----- */



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
    HITEM  *ph,*phtmp, *hp;
    ub4     y, _new_hval, _new_hjval;
    HSLAB *hs, *hsp;
    int i, m;

    if(hdr == NULL) return -1;
    if(hdr->skl > LIMIT_SLAB_BYTE) return -1;

    _new_hval = lookup(hdr->key,hdr->keyl,0);
    _new_hjval = jenkins_one_at_a_time_hash(hdr->key, hdr->keyl);
    
    ph = pools_hitem[(y=(_new_hval&pools_tab->mask))];    
    
    i = hsms(hdr->drl);
    m = 0;
    phtmp = ph;
    ph = ph->next;  /* head is not store anything */
    while ( ph ){
        phtmp = ph; 
        if ((_new_hval == ph->hval) && 
            (hdr->keyl == ph->keyl) && 
            (_new_hjval == ph->hjval))
        {
            m = hsms(ph->psize);
            
            if(m != i){  /* old size != new size , free old slab */
                addfslab(ph->psize, ph->sid, ph->sa);  /* free old slab */

                ph->psize = slabclass[i].size;  /* update psize */
                FSLAB *fslab = findslab(hp->psize);

                hp->sid = fslab->sid;
                hp->sa = fslab->sa;
               
            }

            hsp = findhslab(i, ph->sid);    
            if(hsp != NULL){
                memcpy(hsp->sm+ph->sa*ph->psize, hdr->dr, hdr->drl);
                return 0;
            }else
                perror("hsp error\n");
            
        }
        ph = ph->next; 
    } /* while */
    
    if(!phtmp->next){
        hp = calloc(1, sizeof(HITEM));
        hp->key   = hdr->sk;
        hp->keyl  = hdr->skl;
        hp->drl = hdr->drl;
        hp->psize = slabclass[i].size;
        hp->sid = ;
        hp->sa = ;
        hp->hval  = hval;
        hp->hjval = hjval;
        hp->utime = 0;
        hp->ahit = 0;
        hp->next = NULL;
        

        FSLAB *fslab = findslab(hp->psize);
        HSLAB *hslab = findhslab(i, fslab->sid); 

        hp->sid = fslab->sid;
        hp->sa = fslab->sa;

        memcpy(hslab->sm+fslab->sa*hp->psize, hdr->dr, hdr->drl);

        phtmp->next = hp;
        
    }
    
    if (++pools_htab->count > (ub4)1<<(pools_htab->logsize))
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
    
    tlist = pools_tlist;
     
    hitem_pool = pools_hitem;
    if(!hitem_pool){
        perror("shmat hitem_pool");
    }
    hp = hitem_pool[y=(hval&pools_htab->mask)];
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
 *         Name:  inithslab
 *  Description:  
 * =====================================================================================
 */
static void inithslab ( int i ){
    int m;
    for(m=0; m<i; m++){ 
        if(pools_hslab[m]){
            pools_hslab[m]->sm = NULL;
            pools_hslab[m]->ss = 0;
            pools_hslab[m]->sf = 0;
            pools_hslab[m]->id = 0;
            pools_hslab[m]->next = NULL;
        }
    }
}		/* -----  end of static function inithslab  ----- */


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
FSLAB *findslab ( sb2 _psize ){
    FSLAB *fslab, *fs_tmp;
    HSLAB *hslab, *hs_tmp;
    int i;

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
                goto loop;
            }
    }
    return NULL;
}		/* -----  end of function findslab  ----- */

 /* vim: set ts=4 sw=4: */

