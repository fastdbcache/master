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
#ifndef dep_mmap.h
#define dep_mmap.h
#ifdef __cplusplus
 extern "C"
 {
#endif

#include "../config_global.h"
#include "dep_struct.h"
#include "../misc.h"

MMPO *mmpo_init (  );
DEST *mmap_init ( size_t byte );
int mmap_set ( ub1 *key, ub4 keyl );
#ifdef __cplusplus
 }
#endif
#endif /* --- #dep_mmap.h ---*/
 /* vim: set ts=4 sw=4: */


