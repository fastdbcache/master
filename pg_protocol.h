/*
 * =====================================================================================
 *
 *       Filename:  pg-protocol3.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/05/2013 10:42:33 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), 
 *   Organization:  
 *
 * =====================================================================================
 */


#ifndef  PG_PROTOCOL_H_
#define  PG_PROTOCOL_H_

#include <sys/types.h>

#include "command.h"
#include "log_lib.h"
#include "socket_lib.h"

#ifndef STANDARD
#include "hashtable/standard.h"
#endif
#ifndef HASHTAB
#include "hashtable/hashtab.h"
#endif
#define LINELEN 10

#define MAX_BACKEND_SESSION 128

/* These are the authentication request codes sent by the backend. */

#define AUTH_REQ_OK         0   /* User is authenticated  */
#define AUTH_REQ_KRB4       1   /* Kerberos V4. Not supported any more. */
#define AUTH_REQ_KRB5       2   /* Kerberos V5 */
#define AUTH_REQ_PASSWORD   3   /* Password */
#define AUTH_REQ_CRYPT      4   /* crypt password. Not supported any more. */
#define AUTH_REQ_MD5        5   /* md5 password */
#define AUTH_REQ_SCM_CREDS  6   /* transfer SCM credentials */
#define AUTH_REQ_GSS        7   /* GSSAPI without wrap() */
#define AUTH_REQ_GSS_CONT   8   /* Continue GSS exchanges */
#define AUTH_REQ_SSPI       9   /* SSPI negotiate without wrap() */


typedef struct _msg_format MSGFORMAT;
struct _msg_format{
    char *format;
    ssize_t format_len;
    MSGFORMAT *next;
};

typedef struct {
   char *StartupPack;
   ssize_t sp_len;
   char *passwd; 
   ssize_t pw_len;
   char *user;
   char *database;
   char *application_name;
   uint64_t major;
   uint64_t minor;
   MSGFORMAT *head;
   MSGFORMAT *tail;
   int backend_fd;
}SESSION_SLOTS;

struct pg_conn {
    int frontend;  /* frontend fd */
    int backend;    /* backend fd */
};				/* ----------  end of struct pg_conn  ---------- */

typedef struct pg_conn Pg_conn;

SESSION_SLOTS *bs_slots[MAX_BACKEND_SESSION];

//htab *cache_hast_table = NULL;


int PGStartupPacket3(int fd, char *pack); /* 1. F -> B */
int PGAuthenticationMD5Password(int fd, char *pack);/* 2. B -> F */
int PGPasswordMessage(int fd, char *pack);/* 3. F -> B */
int PGAuthenticationOk(int fd, char *pack);/* 4.  B -> F */
int PGParameterStatus(int fd, char *pack);/* 5. B -> F */
int PGBackendKeyData(int fd, char *pack);/* 6. B -> F */
int PGReadyForQuery(int fd, char *pack);/* 7. B -> F */
int PGSimpleQuery(int fd, char *pack); /* 8. F -> B */
int PGExchange2(const int bfd,const int ffd);
int PGExchange(const int bfd,const int ffd, SESSION_SLOTS *slot);
SESSION_SLOTS *resolve_slot(const char *buf);
int AuthPG(const int bfd,const int ffd, SESSION_SLOTS *slot);
#endif   /* PG_PROTOCOL_H_*/
/* vim: set ts=4 sw=4: */

