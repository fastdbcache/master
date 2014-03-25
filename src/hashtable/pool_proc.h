/*
 * =====================================================================================
 *
 *       Filename:  pool_proc.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/11/2013 03:25:13 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef POOL_PROC_H
#define POOL_PROC_H
#ifdef __cplusplus
 extern "C"
 {
#endif

#include "pool_struct.h"
#include "hashtab.h"

void hproc ( );
void fetchdti (  );
word haddHitem ( HDR *mhdr );
void hrule ( HITEM *hitem, int isize, int x, int y, int id );
void droprule ( int isize, HITEM *_ph );

#ifdef __cplusplus
 }
#endif
#endif /* --- #POOL_PROC_H ---*/
 /* vim: set ts=4 sw=4: */

