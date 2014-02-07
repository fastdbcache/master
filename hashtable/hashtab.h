/*
 * =====================================================================================
 *
 *       Filename:  hashtab.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/25/2013 06:00:04 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef HASHTAB_H
#define HASHTAB_H
#ifdef __cplusplus
 extern "C"
 {
#endif

#include <stdio.h>
#include <math.h>

#include "pool_struct.h"
#include "pool_init.h"

void hgrow();
HSLAB *findhslab ( ssize_t i, sb2 _sid);
int hsms ( ub4 bytes );
void addfslab ( HITEM *_ph);
FSLAB *findfslab ( sb2 _psize );
int findslab ( sb2 _psize );

#ifdef __cplusplus
 }
#endif
#endif /* --- #HASHTAB_H ---*/
 /* vim: set ts=4 sw=4: */

