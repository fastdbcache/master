/*
 * =====================================================================================
 *
 *       Filename:  CONFIG_GLOBAL.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/20/2013 03:21:18 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef CONFIG_GLOBAL_H
#define CONFIG_GLOBAL_H
#ifdef __cplusplus
 extern "C"
 {
#endif

# ifndef STDIO
#  include <stdio.h>
#  define STDIO
# endif
# ifndef STDDEF
#  include <stddef.h>
#  define STDDEF
# endif
#include <sys/types.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h> 

#include "conf_lib.h"

typedef unsigned short uint16;
typedef unsigned int uint32;
typedef  unsigned long  ub8;
#define UB8MAXVAL 0xffffffffffffffffLL
#define UB8BITS 64
typedef    signed long  sb8;
#define SB8MAXVAL 0x7fffffffffffffffLL
typedef  unsigned long  int  ub4;   /* unsigned 4-byte quantities */
#define UB4MAXVAL 0xffffffff
typedef    signed long  int  sb4;
#define UB4BITS 32
#define SB4MAXVAL 0x7fffffff
typedef  unsigned short int  ub2;
#define UB2MAXVAL 0xffff
#define UB2BITS 16
typedef    signed short int  sb2;
#define SB2MAXVAL 0x7fff
typedef  unsigned       char ub1;
#define UB1MAXVAL 0xff
#define UB1BITS 8
typedef    signed       char sb1;   /* signed 1-byte quantities */
#define SB1MAXVAL 0x7f
typedef                 int  word;  /* fastest type available */

#define MAX_HITEM_LENGTH 1024
#define MAX_HITEM_LENGTH_8 (MAX_HITEM_LENGTH<<8)
#define MAX_HARU_POOL 1024
#define MAX_SLAB_CLASS  200
#define CHUNK_ALIGN_BYTES 8
#define MAX_SLAB_BYTE 128*1024 * 1024
#define LIMIT_SLAB_BYTE 1024 * 1024
#define SLAB_BEGIN 88
#define LIMIT_PERCENT 0.1

#define MD5_LENG 33

#define bis(target,mask)  ((target) |=  (mask))
#define bic(target,mask)  ((target) &= ~(mask))
#define bit(target,mask)  ((target) &   (mask))
#ifndef min
# define min(a,b) (((a)<(b)) ? (a) : (b))
#endif /* min */
#ifndef max
# define max(a,b) (((a)<(b)) ? (b) : (a))
#endif /* max */
#ifndef align
# define align(a) (((ub4)a+(sizeof(void *)-1))&(~(sizeof(void *)-1)))
#endif /* align */
#ifndef abs
# define abs(a)   (((a)>0) ? (a) : -(a))
#endif
#define TRUE  1
#define FALSE 0
#define SUCCESS 0  /* 1 on VAX */

#define DEBUG(fmt, args...) printf(fmt" %s %d\n", ##args, __FILE__, __LINE__)

#define SPACE(point) do{ \
    while (*(point) == ' ' || *(point) == '\t' || *(point) == '\r' || *(point) == '\n'){  \
        (point)++;                                                    \
    }                                                           \
}while(0)

#define SPACER(point, move) do{ \
    while (*(point) == ' ' || *(point) == '\t' || *(point) == '\r' || *(point) == '\n'){  \
        (point)++;                                                    \
        (move)++;                                                   \
    }                                                           \
}while(0)

typedef enum {
    H_TRUE=0,
    H_FALSE
} H_STATE;

typedef enum {
    H_USE=0,
    H_FREE
} H_USESTAT;

typedef enum {
    D_MEM=0,    /* memory */
    D_MMAP,     /* mmap  */
    D_HD        /* hard disk */
} D_TYPE;

typedef enum {
    H_INSERT=0,  /*  default 0 ,it's 1 has a job, 2 working */
    H_UPDATE
} H_CHANGE; 

typedef struct __ly _ly;
struct __ly {
    char *tab;
    int len;
    _ly *next;
};

typedef struct __packer DBP;
struct __packer{
    char *inBuf;
    size_t 		inBufSize;		/* allocated size of buffer */
	int			inStart;		/* offset to first unconsumed data in buffer */
	int			inCursor;		/* next byte to tentatively consume */
	int			inEnd;			/* offset to first position after avail data */

    int backend;
    int frontend;
};

typedef struct __deporule DEPR;
struct __deporule{
    char *table;
    size_t  len;		/* length for table */
    DEPR *next;
};

/* 
typedef struct {
   DBP *StartupPack;
   DBP *verify; 
   H_STATE doing;
   ssize_t pw_len;
   char *user;
   char *database;
   char *application_name;
   uint64_t major;
   uint64_t minor;   
   int backend_fd;
}SESSION_SLOTS;

SESSION_SLOTS *conn_session_slot;
pthread_mutex_t session_slot_lock;

#define SLOT_LOCK() do{\
    pthread_mutex_lock(&session_slot_lock); \
}while(0)

#define SLOT_UNLOCK() do{\
    pthread_mutex_unlock(&session_slot_lock); \
}while(0)
 */
typedef struct __conn _conn;
struct __conn{
    char *fdbc;  /* version for fastdbcache */

    char *server_ip;   /* server listen ip */
    ssize_t server_port; /* server listen port */

    ssize_t unix_sock; /* is socket */
    char *path;   /* socket path */

    char *pg_host;  /* db host */
    ssize_t pg_port;  /* db port */

    ssize_t do_daemonize; /* is daemon */

    char *pid_file;  /* pid path */
    
    ssize_t process_num;  /* work process number */
    ssize_t max_link;

    float factor;       /* for hslab default 1.25  */
    size_t maxbytes;   /* max bytes for hslab */
    
    D_TYPE deptype;     /* deposit type */
    size_t dmaxbytes;   /* max bytes for deposit  */
    H_STATE hasdep;      /* is use deposit default FALSE */
    size_t quotient;    /* when client over quotient, deposit inure */
    DEPR  **deprule;    /* table for deposit */
};

_conn *conn_global;

void conn_init_global();
void conn_get_global ();

#ifdef __cplusplus
 }
#endif
#endif /* --- #CONFIG_GLOBAL_H ---*/
 /* vim: set ts=4 sw=4: */

