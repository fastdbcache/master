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

#include "conf_lib.h"
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

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

typedef struct __conn _conn;
struct __conn{
    char *fdbc;

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

    float factor;
    size_t maxbytes;
};

_conn *conn_global;

void conn_init_global();
void conn_get_global ();
#ifdef __cplusplus
 }
#endif
#endif /* --- #CONFIG_GLOBAL_H ---*/
 /* vim: set ts=4 sw=4: */

