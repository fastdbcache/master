/*
 * =====================================================================================
 *
 *       Filename:  dep_mmap.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/25/2014 01:59:26 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), vyouzhi@gmail.com
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
#ifndef dep_mmap.h
#define dep_mmap.h
#ifdef __cplusplus
 extern "C"
 {
#endif

#include "dep_struct.h"
#include "../misc.h"


MMPO *mmpo_init (  );
DEST *mmap_init ( size_t byte );
int mmap_set ( ub1 *key, ub4 keyl );
void *mmapdb ( uint32 *sid, uint32 id );
#ifdef __cplusplus
 }
#endif
#endif /* --- #dep_mmap.h ---*/
 /* vim: set ts=4 sw=4: */
