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

struct _help_cmd{
    char *cmd;
    char *desc;
};
typedef struct _help_cmd HELP_CMD;


#define COUNT(a, b) do{\
    ssize_t shift=1;        \
    (b) = 0;            \
    do {\
        shift *= 10;        \
        (b)++;          \
    } while((shift*10) < (a));\
}while(0)

void getVer ( int frontend );
void gethtabstat ( int frontend );
void RowHtab (int frontend , ssize_t nfields);
void fdbcHelp ( int frontend );
void RowHelp ( HELP_CMD *_helps, int frontend , ssize_t nfields );
void getItemStat ( char *key, ssize_t keyl, int frontend );
int RowDesLen ( char **parastat, ssize_t *count );
void setRowDescription ( char **parastat, char *crd, ssize_t count, ssize_t nfields );
int RowItem ( char *key, ssize_t keyl, int frontend , ssize_t nfields );
void setCacheRowDescriptions ( int frontend );

void CommandComplete ( ssize_t rows, int frontend );
void ReadyForQuery ( int frontend );

#ifdef __cplusplus
 }
#endif
#endif /* --- #POOL_DEMAND_H ---*/
 /* vim: set ts=4 sw=4: */

