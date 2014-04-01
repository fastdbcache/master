/*
 * =====================================================================================
 *
 *       Filename:  cluster_struct.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/25/2014 04:22:33 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef CLUSTER_STRUCT_H
#define CLUSTER_STRUCT_H
#ifdef __cplusplus
 extern "C"
 {
#endif

#include "../config_global.h"
#include "../socket_lib.h"
#include "../json.h"

struct __cdbl {
};				/* ----------  end of struct __cdbl  ---------- */

typedef struct __cdbl _CDBL;


enum __roles {
    R_LEADER=0,
    R_PROPOSER,
    R_VOTER,
    R_NODE
};				/* ----------  end of enum __roles  ---------- */

typedef enum __roles _Roles;

_Roles clu_roles;
int clu_lead_fd;


void clu_init (  );

#ifdef __cplusplus
 }
#endif
#endif /* --- #CLUSTER_STRUCT_H ---*/
 /* vim: set ts=4 sw=4: */

