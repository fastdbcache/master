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
    //table = (hitem **)malloc(sizeof(hitem *)*(ub4)len);

    if ((hitem_semid = shmget(IPC_PRIVATE, sizeof(HITEM*)*(ub4)len, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    };

    table = (HITEM**)(shmat(hitem_semid, (void *) 0, 0));

    for (i=0; i<len; ++i) table[i] = (HITEM *)0;


    if ((htab_semid = shmget(IPC_PRIVATE, sizeof(HTAB), IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    };

   
    htab = (HTAB *)shmat(htab_semid, (void *) 0, 0);

    htab->logsize = isize;
    htab->mask = len-1;
    htab->count = 0;
    htab->apos = (ub4)0;
    htab->bcount = 0;

}		/* -----  end of function hcreate  ----- */



/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hadd
 *  Description:  
 * =====================================================================================
 */
word hadd (ub1 key,ub4 keyl,void *stuff ){
    
    HTAB *t;
    HITEM  *h,*hp, **table;
    ub4     y, x = lookup(key,keyl,0);
    
    
    t = (HTAB *)shmat(htab_semid, (void *) 0, 0);
    table = (HITEM **)shmat(hitem_semid, (void *)0, 0);

    /* make sure the key is not already there 
    for (h = table[(y=(x&t->mask))]; h; h = h->next)
    {*/

    h = table[(y=(x&t->mask))];    
    

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
}		/* -----  end of function hadd  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hlru
 *  Description: LRU
 * =====================================================================================
 */
void hlru (  ){

    return ;
}		/* -----  end of function hlru  ----- */

 /* vim: set ts=4 sw=4: */

