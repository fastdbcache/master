/*
 * =====================================================================================
 *
 *       Filename:  pg_misc.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/06/2014 12:26:58 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef pg_misc_H
#define pg_misc_H
#ifdef __cplusplus
 extern "C"
 {
#endif

#include "config_global.h"

DBP *initdbp (  );
void freedbp ( DBP *_dbp );
int CheckBufSpace ( ssize_t endPos, DBP *_dbp );
int getInt ( int *result, size_t bytes, DBP *_dbp );

#ifdef __cplusplus
 }
#endif
#endif /* --- #pg_misc_H ---*/
 /* vim: set ts=4 sw=4: */

