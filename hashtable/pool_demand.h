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

void setCacheRowDescriptions ( int frontend );
#ifdef __cplusplus
 }
#endif
#endif /* --- #POOL_DEMAND_H ---*/
 /* vim: set ts=4 sw=4: */

