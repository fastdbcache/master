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
/*    Copyright 2009 10gen Inc.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
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
#include <netinet/in.h>
#include <pthread.h>
#include <malloc.h>
#include <ctype.h>
#include <stdint.h> 
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

#include "pg_system_atalogs.h"
#include "conf_lib.h"
#include "func.h"
#include "log_lib.h"

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
/*typedef  unsigned       char ub1;  */
typedef         char ub1;
#define UB1MAXVAL 0xff
#define UB1BITS 8
typedef    signed       char sb1;   /* signed 1-byte quantities */
#define SB1MAXVAL 0x7f
typedef                 int  word;  /* fastest type available */

#define KEY_LENGTH (1024)
#define MAX_HITEM_LENGTH 16
#define MAX_HITEM_LENGTH_8 (MAX_HITEM_LENGTH<<8)
#define MAX_HG_LENGTH 8
#define MAX_SLAB_CLASS  200
#define MAX_HARU_POOL ( MAX_SLAB_CLASS * 2 )
#define CHUNK_ALIGN_BYTES 8
#define LIMIT_SLAB_BYTE ( 1024 * 1024 )
#define MAX_SLAB_BYTE ( 128 * LIMIT_SLAB_BYTE )
#define DEFAULT_MEM_BYTE ( 1 * LIMIT_SLAB_BYTE )
#define SLAB_BEGIN 88
#define LIMIT_PERCENT 0.1
#define FSLAB_OFFSET (sizeof(uint32)*2)

#define MD5_LENG 33

#define MAXCONN 1024

#define MAXRLIM 1024
#define MAXRQ MAXCONN
#define LIMITROW 30
#define ERR_LENGS 200
#define ERR_ROW 10


#define FILE_PATH_LENGTH 256
#define LIMIT_MMAP_BYTE ( 1024 * 1024 )
#define DEFAULT_MMAP_BYTE ( 1 * LIMIT_MMAP_BYTE )

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

#define DEBUGS 1

#ifdef DEBUGS
    #define DEBUG(fmt, args...) printf(fmt" %s %d\n", ##args, __FILE__, __LINE__)
#else
    #define DEBUG(fmt, args...) ELOG_DEBUG(fmt, args...)
#endif

#define FLOG_ALERT(fmt, args...)   eprintf(err_levels[ELOG_ALERT], fmt" %s %d", ##args, __FILE__, __LINE__ )
#define FLOG_ERR(fmt, args...)     eprintf(err_levels[ELOG_ERR], fmt" %s %d", ##args, __FILE__, __LINE__ )          
#define FLOG_WARN(fmt, args...)    eprintf(err_levels[ELOG_WARN], fmt" %s %d", ##args, __FILE__, __LINE__ ) 
#define FLOG_NOTICE(fmt, args...)  eprintf(err_levels[ELOG_NOTICE], fmt" %s %d", ##args, __FILE__, __LINE__ ) 
#define FLOG_INFO(fmt, args...)    eprintf(err_levels[ELOG_INFO], fmt" %s %d", ##args, __FILE__, __LINE__ )
#define FLOG_DEBUG(fmt, args...)   eprintf(err_levels[ELOG_DEBUG], fmt" %s %d", ##args, __FILE__, __LINE__ ) 


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
    E_SQL_TYPE stype; 
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


struct __error_record {
    char error[ERR_LENGS];
    char etime[26];
};				/* ----------  end of struct __error_record  ---------- */

typedef struct __error_record ERREC;

struct __query_error {
    ERREC *errs;
    int offset;
};				/* ----------  end of struct __query_error  ---------- */

typedef struct __query_error QERR;
QERR *pools_qerr;
pthread_mutex_t query_error_lock;

struct __fd
{
    int fd;
    size_t fsize;
    char name[FILE_PATH_LENGTH];
    int len;
    char *ptr;
    struct __fd *next;
};
typedef struct __fd HFD;
HFD *pools_hfd;

#define TAIL_HFD(tail) do{\
    HFD *_nhfd;             \
    _nhfd = pools_hfd;      \
    while(_nhfd && _nhfd->next){     \
        _nhfd = _nhfd->next;\
    }                       \
    (tail) = _nhfd;         \
}while(0)

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
    int maxrlim;  /* rlimit */

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

/* hashtable */
    float factor;       /* for hslab default 1.25  */
    int  max_slab;      /* count slab */
    ssize_t maxbytes;   /* max bytes for hslab */
    int delaytime;     /* delay time for update */
    D_TYPE cache_method;  /* use mem or mmap, default D_MEM */ 
    size_t default_bytes; /* for mem or mmap */
    size_t chunk_bytes;  /* maxbytes / default_bytes */
/* end hashtable */

    D_TYPE deptype;     /* deposit type */
    ssize_t dmaxbytes;   /* max bytes for deposit  */
    H_STATE hasdep;      /* is use deposit default FALSE */
    ssize_t quotient;    /* when client over quotient, deposit inure */
    /*DEPR  **deprule;     table for deposit */
    char *deprule;

    char *mmap_path;
    ssize_t mmdb_length;

    sb2 maxrq;
    sb2 limit_rows;    /* max return limit rows */
};

_conn *conn_global;

void conn_init_global();
void conn_get_global ();
void initDeposit ( );
void pathCheck ( );
ub4 alignByte ( ub4 len );


#define QERR_LOCK() do{\
    pthread_mutex_lock(&query_error_lock); \
}while(0)

#define QERR_UNLOCK() do{\
    pthread_mutex_unlock(&query_error_lock); \
}while(0)
#ifdef __cplusplus
 }
#endif
#endif /* --- #CONFIG_GLOBAL_H ---*/
 /* vim: set ts=4 sw=4: */

