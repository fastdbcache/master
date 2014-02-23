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
/*    Copyright 2009 10gen Inc.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
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
void unmmap ( const char *pathname );
HFD *inithfd ( );
void freehfd ( HFD *_hfd );

#ifdef __cplusplus
 }
#endif
#endif /* --- #MISC_H ---*/
 /* vim: set ts=4 sw=4: */

