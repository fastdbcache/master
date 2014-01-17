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
#include <string.h>

#include "socket_lib.h"
#include "thread_lib.h"
#include "./hashtable/pool_init.h"
#include "./hashtable/pool_worker.h"
#include "./hashtable/pool_demand.h"
#include "./deposit/dep_struct.h"
#include "pg_system_atalogs.h"
#include "pg_misc.h"
#include "log_lib.h"

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

int PGStartupPacket3(int fd, DBP *_dbp); /* 1. F -> B 
SESSION_SLOTS *resolve_slot(const char *buf);*/
int AuthPG(const int bfd,const int ffd);
E_SQL_TYPE findSQL (  const char *sql , int len);
E_SQL_TYPE findCache ( const char *spl, int *offset);

#endif   /* PG_PROTOCOL_H_*/
/* vim: set ts=4 sw=4: */

