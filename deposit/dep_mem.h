/*
 * =====================================================================================
 *
 *       Filename:  dep_misc.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年01月05日 02时03分06秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), vyouzhi@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef DEP_MISC_H
#define DEP_MISC_H
#ifdef __cplusplus
 extern "C"
 {
#endif

#include "../config_global.h"
#include "dep_struct.h"
#include "../hashtable/pool_worker.h"

DEST *mem_init ( size_t byte );
int mem_set ( ub1 *key, ub4 keyl );
int mem_pushdb ( DBP *_dbp );

#ifdef __cplusplus
 }
#endif
#endif /* --- #DEP_MISC_H ---*/
 /* vim: set ts=4 sw=4: */


