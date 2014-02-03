/*
 * =====================================================================================
 *
 *       Filename:  pool_init.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/11/2013 10:05:41 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "pool_init.h"

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hcreate
 *  Description:  
 * =====================================================================================
 */
void hcreate ( int isize ){
    int i;
    ub4 len;
    int max_slab, count_hslab;
    HFD *_hfd, *_hfd_next;
    char htab_path[1024];

    len = ((ub4)1<<isize);

#ifdef MMAP
    pools_hfd = inithfd();
    if(!pools_hfd){
        DEBUG("init pools_hfd error");
        exit(-1);
    } 
    _hfd_next = pools_hfd;
#endif

#ifdef MMAP
    
    bzero(htab_path, 1024);
    snprintf(htab_path, 1023, "%s/%s",conn_global->mmap_path,HashTable_for_list[0]);
    _hfd = inithfd();
    if(!_hfd){
        DEBUG("init inithfd error");
        exit(-1);
    }
    _hfd_next->next = _hfd;
    _hfd_next = _hfd;
    pools_htab = (HTAB *)mcalloc(1, sizeof(HTAB),htab_path,O_RDWR|O_CREAT, _hfd);
#else
    pools_htab = (HTAB *)calloc(1, sizeof(HTAB));
#endif
    if(pools_htab == NULL){
        DEBUG("pools_htab calloc error");
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
    bzero(pools_htab->hslab_stat,  sizeof(pools_htab->hslab_stat));

#ifdef MMAP
    bzero(htab_path, 1024);
    snprintf(htab_path, 1023, "%s/%s",conn_global->mmap_path,HashTable_for_list[1]);
    _hfd = inithfd();
    if(!_hfd){
        DEBUG("init inithfd error");
        exit(-1);
    }
    _hfd_next->next = _hfd;
    _hfd_next = _hfd;
    pools_harug = (HARUG *)mcalloc(1, sizeof(HARUG)*sizeof(HARU)*MAX_HARU_POOL,htab_path,O_RDWR|O_CREAT, _hfd);
#else
    pools_harug = (HARUG *)calloc(1, sizeof(HARUG));
#endif
    pools_harug->step = 0;
    for(i=0; i < MAX_HARU_POOL; i++){
        /*pools_harug->pools_haru_pool[i] = (HARU *)calloc(1, sizeof(HARU));
        if(pools_harug->pools_haru_pool[i] == NULL){
            perror("pools_haru_pool callo error ");
            exit(1);
        }*/
        pools_harug->haru_pool[i].phitem = NULL;
    
    }
    pools_haru_pool = pools_harug->haru_pool;
   
#ifdef MMAP
    bzero(htab_path, 1024);
    snprintf(htab_path, 1023, "%s/%s",conn_global->mmap_path,HashTable_for_list[2]);
    _hfd = inithfd();
    if(!_hfd){
        DEBUG("init inithfd error");
        exit(-1);
    }
    _hfd_next->next = _hfd;
    _hfd_next = _hfd;
    pools_hitem_row = (ub4 *)mcalloc(len, sizeof(ub4),htab_path,O_RDWR|O_CREAT, _hfd);
#else 
    pools_hitem_row = calloc(len, sizeof(ub4));
#endif

    hitem_group = (HG *)calloc(1, sizeof(HG));
    hitem_group->bucket = pools_htab->logsize;
    hitem_group->usable = inithitem ( (ub4) len );
    hitem_group->move = NULL;

    pools_hdr_head = hdrcreate();
    pools_hdr_tail = pools_hdr_head;

#ifdef MMAP
    bzero(htab_path, 1024);
    snprintf(htab_path, 1023, "%s/%s",conn_global->mmap_path,HashTable_for_list[3]);
    _hfd = inithfd();
    if(!_hfd){
        DEBUG("init inithfd error");
        exit(-1);
    }
    _hfd_next->next = _hfd;
    _hfd_next = _hfd;
    pools_tlist = (TLIST *)mcalloc(1, sizeof(TLIST)*sizeof(char)*KEY_LENGTH,htab_path,O_RDWR|O_CREAT, _hfd);
#else
    pools_tlist = (TLIST *)calloc(1, sizeof(TLIST));
#endif
    max_slab = hslabclass();

    count_hslab = conn_global->maxbytes / LIMIT_SLAB_BYTE;
    if(count_hslab < 1){
        count_hslab = 1; 
    }
    inithslab ( count_hslab );

    /*pthread_mutex_init(&work_lock_fslab, NULL);  */
    pthread_mutex_init(&work_lock_hit, NULL);
    pthread_mutex_init(&work_lock_miss, NULL);
    pthread_mutex_init(&work_lock_bytes, NULL);
    pthread_mutex_init(&work_lock_tlist, NULL);
    pthread_mutex_init(&work_lock_hdr, NULL);

#ifdef MMAP
    bzero(htab_path, 1024);
    snprintf(htab_path, 1023, "%s/%s",conn_global->mmap_path,HashTable_for_list[5]);
    _hfd = inithfd();
    if(!_hfd){
        DEBUG("init inithfd error");
        exit(-1);
    }
    _hfd_next->next = _hfd;
    _hfd_next = _hfd;
    pools_fslab = (FSLAB *)mcalloc(max_slab, sizeof(FSLAB),htab_path,O_RDWR|O_CREAT, _hfd);
#else
    pools_fslab = (FSLAB *)calloc(max_slab, sizeof(FSLAB));
#endif
    for(i=0; i< max_slab; i++){
        pools_fslab[i].psize = 0;
        pools_fslab[i].sid = 0;
        pools_fslab[i.]sa = 0;    
    }
    return ;
}		/* -----  end of function hcreate  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  inithslab
 *  Description:  
 * =====================================================================================
 */
void inithslab ( int i ){
    int m;

#ifdef MMAP
    bzero(htab_path, 1024);
    snprintf(htab_path, 1023, "%s/%s",conn_global->mmap_path,HashTable_for_list[4]);
    _hfd = inithfd();
    if(!_hfd){
        DEBUG("init inithfd error");
        exit(-1);
    }
    _hfd_next->next = _hfd;
    _hfd_next = _hfd;
    pools_hslab = (HSLAB *)mcalloc(i, sizeof(HSLAB)*sizeof(char)*KEY_LENGTH,htab_path,O_RDWR|O_CREAT, _hfd);
#else
    pools_hslab = (HSLAB *)calloc(i, sizeof(HSLAB));
#endif

    for(m=0; m<i; m++){ 
        pools_hslab[m].sm=NULL;
        pools_hslab[m].ss=0;
        pools_hslab[m].sf=0;
        pools_hslab[m].id=0;
    }

}		/* -----  end of static function inithslab  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hslabnull
 *  Description:  
 * =====================================================================================
 */
HSLAB *hslabnull (  ){
    HSLAB *_h;
    _h = (HSLAB *)calloc(1, sizeof(HSLAB));
    if(_h != NULL){
        _h->sm = NULL;
        _h->ss = 0;
        _h->sf = 0;
        _h->id = 0;
        _h->next = NULL;
    }
    return _h;
}		/* -----  end of function hslabnull  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hslabcreate
 *  Description:  
 * =====================================================================================
 */
HSLAB *hslabcreate ( int i ){
    HSLAB *h;

    h = hslabnull();
    h->sm = (ub1 *)calloc(LIMIT_SLAB_BYTE, sizeof(ub1));
    if(!h->sm)perror("h sm calloc error\n"); 
    h->sf = slabclass[i].chunk;
   
    pools_htab->bytes += LIMIT_SLAB_BYTE;    
    pools_htab->hslab_stat[i]++;
     
    return h;
}		/* -----  end of function hslabcreate  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hslabclass
 *  Description:  
 * =====================================================================================
 */
int hslabclass ( void ){
    int size = SLAB_BEGIN;
    int i=0;
    
    while (i < MAX_SLAB_CLASS && size <= MAX_SLAB_BYTE / conn_global->factor) {
        
        size = alignByte(size);
        /*if((item_size_max / size) < 2) break; 

        printf("slab class  chunk size %9u perslab %7u\n",
                     size, (MAX_SLAB_BYTE / size));*/
        slabclass[i].size = size;
        slabclass[i].chunk = (MAX_SLAB_BYTE / size);
        /*printf("slabclass: size:%d, chunk:%d\n", slabclass[i].size, slabclass[i].chunk);*/
        i++;
        size *= conn_global->factor;
    } 
    slabclass[i].size = MAX_SLAB_BYTE;
    slabclass[i].chunk = 1;

    return (++i);
}		/* -----  end of function hslabclass  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  inithitem
 *  Description:  
 * =====================================================================================
 */
HITEM **inithitem ( ub4 len ){
    int i;
    HITEM **hitem;
    hitem = (HITEM **)calloc(len, sizeof(HITEM));
    if(hitem == NULL) return NULL;
    /* pools_hitem head not store anything */
    for (i=0; i<len; ++i) {
        hitem[i] = hitemcreate();
    }
    return hitem;
}		/* -----  end of function inithitem  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hitemcreate
 *  Description:  
 * =====================================================================================
 */

HITEM *hitemcreate(){
    HITEM *h;
    h = (HITEM *)calloc(1, sizeof(HITEM));
    if(h){
        h->key = NULL;
        h->keyl = 0;
        h->drl = 0;
        h->psize = 0;
        h->sid = 0;
        h->sa = 0;
        h->hval = 0;
        h->hjval = 0;
        h->utime = 0;
        h->offtime = 0;
        h->ahit = 0;  
        h->amiss = 0;
        h->next = NULL;
    }
    return h;
}		/* -----  end of function hitemcreate  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  freehitem
 *  Description:  
 * =====================================================================================
 */
void freehitem ( HITEM **_h, ub4 len ){
    int i;

    if(!*_h) return;
    if(!_h) return;

    for(i=0; i<len; i++){
        if(!_h[i]) free(_h[i]);
    }

    free(*_h);

}		/* -----  end of function freehitem  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hdrcreate
 *  Description:  
 * =====================================================================================
 */
HDR *hdrcreate (  ){
    HDR *d;
    d = (HDR *)calloc(1, sizeof(HDR));
    if(d){
        d->key = NULL;
        d->keyl = 0;
        d->stime = 0;
        d->flag = H_TRUE;
        d->dr = NULL;
        d->drl = 0;
        d->next = NULL;            
    }
    return d;
}		/* -----  end of function hdrcreate  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  initulist
 *  Description:  
 * =====================================================================================
 */
ULIST *initulist (  ){
    ULIST *u;

    u = (ULIST *)calloc(1, sizeof(ULIST));
    if(u){
        u->key = NULL;
        u->keyl = 0;
        u->utime = 0;
        u->flag = H_TRUE;
        u->next = NULL;
    }
    
    return u;
}		/* -----  end of function initulist  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  mcalloc
 *  Description:  
 * =====================================================================================
 */
void *mcalloc ( size_t nmemb, size_t size, const char *pathname, int flags, HFD *hfd ){
    int fd;
    void *start;
    struct stat sb;
    char name[1];

    fd = open(pathname, flags);
    fstat(fd, &sb);
    if(sb.st_size==0){
        DEBUG("init mmap file");
        name[0]='\0';
        write(fd, name, nmemb*size);
        lseek(fd,0,SEEK_SET);
    }
    fstat(fd, &sb);
    start = mmap(NULL, sb.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(start == MAP_FAILED){
        DEBUG("init mmap error");
        close(fd);
        return NULL;
    }
    hfd->fd = fd;

    return start;
}		/* -----  end of function mcalloc  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  inithfd
 *  Description:  
 * =====================================================================================
 */
HFD *inithfd ( ){
    HFD *_hfd;

    _hfd = (HFD *)calloc(1, sizeof(HFD));
    if(!_hfd) return NULL;
    _hfd->fd = 0;
    
    return _hfd;
}		/* -----  end of function inithfd  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  freehfd
 *  Description:  
 * =====================================================================================
 */
void freehfd ( HFD *_hfd ){
    if(_hfd->next){
        freehfd(_hfd->next);
    }
    close(_hfd->fd);
    free(_hfd);
}		/* -----  end of function freehfd  ----- */

 /* vim: set ts=4 sw=4: */

