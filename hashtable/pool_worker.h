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

void listHslab (  );
void hkey ( char *key, ub4 keyl, SLABPACK *dest);
HITEM *hfind ( char *key, ub4 keyl );
void getslab ( HITEM * hitem, SLABPACK *dest);
int addHdr ( HDR *myhdr );
int addUlist ( ULIST *mlist);
void freeHdr ( HDR *fhdr );
void freeUList ( ULIST *flist );

#ifdef __cplusplus
 }
#endif
#endif /* --- #POOL_WORKER_H ---*/
 /* vim: set ts=4 sw=4: */

