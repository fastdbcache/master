/*
 * =====================================================================================
 *
 *       Filename:  pool_demand.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/30/2013 08:41:23 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef POOL_DEMAND_H
#define POOL_DEMAND_H
#ifdef __cplusplus
 extern "C"
 {
#endif

#include <stdlib.h>
#include "pool_struct.h"
#include "hashtab.h"
#include "../socket_lib.h"

char *item[]={"key","utime","ahit","amiss",NULL};
char *version[]={"version",NULL};
char *help[]={"help",NULL};
char *stat[]={"count", "bcount","lcount","hit","miss","set","get","bytes",NULL};
char *hstat[MAX_SLAB_CLASS];

void setCacheRowDescriptions ( int frontend );
#ifdef __cplusplus
 }
#endif
#endif /* --- #POOL_DEMAND_H ---*/
 /* vim: set ts=4 sw=4: */

