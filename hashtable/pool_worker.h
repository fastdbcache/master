/*
 * =====================================================================================
 *
 *       Filename:  pool_worker.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/11/2013 11:16:42 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef POOL_WORKER_H
#define POOL_WORKER_H
#ifdef __cplusplus
 extern "C"
 {
#endif

#include "pool_struct.h"
#include "hashtab.h"

char *hkey(char *key, ub4 keyl);
HITEM *hfind ( char *key, ub4 keyl );
char *getslab ( HITEM * hitem);
int addHdr ( HDR *myhdr, int m );
int addUlist ( ULIST *mlist, int m );
void freeHdr ( HDR *fhdr );
void freeUList ( ULIST *flist );

#ifdef __cplusplus
 }
#endif
#endif /* --- #POOL_WORKER_H ---*/
 /* vim: set ts=4 sw=4: */

