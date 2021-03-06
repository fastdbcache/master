/*
 * =====================================================================================
 *
 *       Filename:  misc.h
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
#ifndef MISC_H
#define MISC_H
#ifdef __cplusplus
 extern "C"
 {
#endif

#include "config_global.h"

DBP *initdbp (  );
void freedbp ( DBP *_dbp );
int CheckBufSpace ( ssize_t endPos, DBP *_dbp );
int getInt ( int *result, size_t bytes, DBP *_dbp );
void *mcalloc ( size_t nmemb, size_t size, const char *pathname, int flags);
HFD *inithfd ( );
void freehfd ( HFD *_hfd );
char *buildCachePath (const char *name );

#ifdef __cplusplus
 }
#endif
#endif /* --- #MISC_H ---*/
 /* vim: set ts=4 sw=4: */

