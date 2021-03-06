/*
 * =====================================================================================
 *
 *       Filename:  pool_init.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/11/2013 10:05:33 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef POOL_INIT_H
#define POOL_INIT_H
#ifdef __cplusplus
 extern "C"
 {
#endif

#include "pool_struct.h"

void hcreate (int isize);
void inithslab ( int i );
HSLAB *hslabnull (  );
HSLAB *hslabcreate ( int i );
int hslabclass ( void );
HITEM **inithitem ( ub4 len );
HITEM *hitemcreate();
void freehitem ( HITEM **_h, ub4 len );
HDR *hdrcreate (  );
ULIST *initulist (  );

#ifdef __cplusplus
 }
#endif
#endif /* --- #POOL_INIT_H ---*/
 /* vim: set ts=4 sw=4: */

