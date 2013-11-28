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
 *         Name:  hinit
 *  Description:  
 * =====================================================================================
 */
void hinit (){
HTAB mem_htab;   /* stat record */
int mem_hslab_stat[MAX_SLAB];
ub4 mem_hitem_row[MAX_HITEM_LENGTH_8];
HARU *mem_haru_POOL[MAX_HITEM_LENGTH];    /* haru  */

int semid_hitem_pool;   /* hitem struct */
int semid_hdr_pool;     /* any work proc of data row */
int semid_tlist;  /* mem proc change, and work proc read table list shm id */
int semid_ulist;  /* any work proc change flag=1, and work proc read from this to tlist */
int semid_hslab_pool;   /* slab leave in data row of the store*/
int semid_fslab;   /* free slab */

    int i;
    HITEM **hitem_pool;

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

    if ((semid_hitem_pool = shmget(IPC_PRIVATE, sizeof(HITEM)*MAX_HITEM_LENGTH_8, IPC_CREAT | 0666)) < 0) {
        perror("shmget semid_hslab_pool");
        exit(1);
    };

    hitem_pool = (HITEM**)(shmat(semid_hitem_pool, (void *) 0, 0));
    if(!hitem_pool){
        perror("shmat hitem_pool");
    }

    for (i=0; i<MAX_HITEM_LENGTH_8; ++i) hitem_pool[i] = (HITEM *)0;

    
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

    if ((semid_hslab_pool = shmget(IPC_PRIVATE, sizeof(HSLAB)*MAX_SLAB, IPC_CREAT | 0666)) < 0) {
        perror("shmget semid_hslab_pool");
        exit(1);
    };

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
            /*  if(hp->utime > ){

            }*/
            return hp;
        }
    }
    
    return NULL;
}		/* -----  end of function hfind  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hslab
 *  Description:  
 * =====================================================================================
 */
char *gslab ( HITEM *hi ){
    HITEM *hp = hi;
    HSLAB **hslab_pool, *slab;

    **hslab_pool = (HSLAB**)(shmat(semid_hslab_pool, (void *) 0, 0));
    if(!hslab_pool){
        perror("shmat hitem_pool");
    }
    
    slab = hslab_pool[]
    return NULL;
}		/* -----  end of function hslab  ----- */



/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hslabclass
 *  Description:  
 * =====================================================================================
 */
void hslabclass ( void ){
    int size = 88;
    int i=0;
    
     while (i++ < MAX_SLAB_CLASS && size <= MAX_SLAB_BYTE / slabclass) {
        if (size % CHUNK_ALIGN_BYTES)
            size += CHUNK_ALIGN_BYTES - (size % CHUNK_ALIGN_BYTES);
        if((item_size_max / size) < 2) break; 

        printf("slab class  chunk size %9u perslab %7u\n",
                     size, (MAX_SLAB_BYTE / size));
        slabclass[i].size = size;
        slabclass[i].chunk = (MAX_SLAB_BYTE / size);

        size *= conn_global->factor;
   } 
}		/* -----  end of function hslabclass  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hsms
 *  Description:  
 * =====================================================================================
 */
int hsms ( ub4 bytes ){
    int i;
    
    for ( i=0; slabclass[i].chunk > 0; i++ ) {
        if(slabclass[i].size > bytes) return i;
    }

    return -1;
}		/* -----  end of function hsms  ----- */

 /* vim: set ts=4 sw=4: */

