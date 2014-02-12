/*
 * =====================================================================================
 *
 *       Filename:  m.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/28/2014 04:33:52 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), vyouzhi@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>

typedef unsigned int uint32;
typedef         char ub1;
typedef  unsigned long  int  ub4;
typedef    signed short int  sb2;
#define UB4MAXVAL 0xffffffff
typedef    signed long  int  sb4;
#define UB4BITS 32
#define SB4MAXVAL 0x7fffffff
typedef  unsigned short int  ub2;
#define MAX_SLAB_CLASS 200
#define MAX_HARU_POOL 10

typedef struct __mms _mms;
struct __mms{
    uint32 total;
    uint32 id;
    uint32 offset;
    uint32 uuid; 
    char *sm;
}; 

struct __hitem
{
  ub1          key[MAX_SLAB_CLASS];      /*  key that is hashed */
  ub4           keyl;     /*  length of key */
  ub4           drl;      /*  length of data row */
  ssize_t           psize;    /*  hpools size */
  sb2           sid;      /*  slab id */
  ub4           sa;       /*  data row start addr of hslab  sa*psize  sa = ss */
  uint64_t          hval;     /*  hash value for key */
  uint64_t           hjval;     /*  hash value for key */
  ub4           utime;    /*  */
  ub4           offtime;  /*  offset update time default 0 */
  ub4           ahit;     /*  all hit */
  ub4           amiss;    /*  all update */
  struct __hitem *next;     /*  next hitem in list */
};
typedef  struct __hitem  HITEM; 

struct __haru
{
  ub1           hid;
  ub1           id;
  ub4           hit;    /*  haru hit */
};
typedef  struct __haru  HARU; 

struct __haru_group
{
  HARU haru_pool[MAX_HARU_POOL];    /*  haru  */
  ssize_t       step;
  ssize_t       max;
  ssize_t       mix;
};
typedef  struct __haru_group  HARUG;


struct __htab
{
  ub4           logsize; /*  log of size of table */
  size_t         mask;    /*  (hashval & mask) is position in table */
  ub4            count;   /*  how many items in this hash table so far? 记录目前使用多少hitem_pool */
  sb2            bcount;  /*  single items length 记录最长的hitem*/
  sb2            lcount;  /*  single items length 记录最短的hitem*/
  ub4            hit;     /*  hval hit times */
  ub4            miss;    /*  hval miss times */
  ub4            set;     /*  total set */
  ub4            get;     /*  total get */
  ub4            bytes;    /*  total data size */
  sb2            hslab_stat[MAX_SLAB_CLASS];  /*  hsalb pool length */
};
typedef  struct __htab  HTAB; 

struct __fd
{
    int fd;
    char name;
    struct __fd *next;
};
typedef struct __fd HFD;
HFD *hfd_pool;

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
        printf("init mmap file\n");
        name[0]='\0';
        write(fd, name, nmemb*size);
        lseek(fd,0,SEEK_SET);
    }
    fstat(fd, &sb);
    printf("file size:%d\n", sb.st_size);
    start = mmap(NULL, sb.st_size, PROT_READ|PROT_WRITE,MAP_SHARED , fd, 0);
    if(start == MAP_FAILED){
        printf("error");
        close(fd);
        return NULL;
    }
    hfd->fd = fd;
    return start;
}		/* -----  end of function mcalloc  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */
int main ( int argc, char *argv[] ){
    _mms *mms;    
    char file[]="/tmp/mmaps", files[]="/tmp/hmmaps";
    uint32 i, l;
    uint32 mm, nn;
    char name[20], cc[]="dddfff";;
    HFD *_hfd;
    //HTAB *_htab;
  //  HITEM *_hitem;
    HARUG *pools_harug;
   // HARU *pools_haru_pool;
    if(cc==NULL){
        #include "mm.h"
        #undef MMAP 
        printf("null\n");    
    }else{
        
        printf("ok\n");
    }

#ifdef MMAP
    printf("mmap\n");
#else
    printf("else\n");
#endif    

    hfd_pool = (HFD *)calloc(1, sizeof(HFD));
    _hfd = (HFD *)calloc(1, sizeof(HFD));
    hfd_pool->next =  _hfd;
   // mms = (_mms **)mcalloc(1024, sizeof(_mms), file, _hfd);
   // _htab = (HTAB *)mcalloc(1, sizeof(HTAB), files, O_RDWR|O_CREAT, _hfd);
   // _hitem = (HITEM *)mcalloc(1, sizeof(HITEM)*strlen(cc), files, O_RDWR|O_CREAT, _hfd);
    pools_harug = (HARUG *)mcalloc(1, sizeof(HARUG)*sizeof(HARU)*MAX_HARU_POOL, files, O_RDWR|O_CREAT, _hfd);
    /*
     
    mms[0].total = 3;
    mms[1].id= 4;
    mms[0].offset=htonl(1);
    mms[2].uuid=7;
    mms[3].id=9;
    mms[6].id=11;
    //sleep(5);
    msync(mms, sizeof(_mms)*1024, MAP_SHARED); 
    printf("totaol:%d, id:%d, offset:%d, uuid:%d\n", mms[0].total, mms[1].id, mms[0].offset, mms[2].uuid); 
    munmap(mms, sizeof(_mms)*10);
    _htab->mask = 22222;
    _htab->miss = 11;
    _htab->hit = 2020;
    _htab->count = 47;
    _htab->hslab_stat[0]=29;
    _htab->hslab_stat[3]=555;
    _htab->hslab_stat[198]=9999;
    printf("miss:%d,hit:%d,hslab_stat[0]:%d, hslab_stat[3]:%d,mask:%d,hslab_stat[192]:%d\n",_htab->miss,_htab->hit,_htab->hslab_stat[0],_htab->hslab_stat[3],_htab->mask,_htab->hslab_stat[198]);
    munmap(_htab, sizeof(HTAB));
    memcpy(_hitem->key, cc, strlen(cc));
    printf("key:%s\n", _hitem->key); 
    munmap(_hitem, sizeof(HITEM)*strlen(cc));

    pools_harug->haru_pool[0].id = 10;
    pools_harug->haru_pool[19].hit = 20;
    pools_harug->max=3;
    pools_harug->mix=1;*/
    printf("haru:%d ,hit:%d,max:%d, mix:%d\n",pools_harug->haru_pool[0].id, pools_harug->haru_pool[19].hit,pools_harug->max, pools_harug->mix);
    munmap(pools_harug, sizeof(HARUG));
    close(hfd_pool->next->fd);
    return EXIT_SUCCESS;
}				/* ----------  end of function main  ---------- */
 /* vim: set ts=4 sw=4: */
