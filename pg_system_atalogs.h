/*
 * =====================================================================================
 *
 *       Filename:  pg_system_atalogs.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/23/2013 03:36:18 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef PG_SYSTEM_ATALOGS_H
#define PG_SYSTEM_ATALOGS_H
#ifdef __cplusplus
 extern "C"
 {
#endif

static const char *Query_for_list[] = {
    "pg_",
   /*   "pg_aggregate",
    "pg_am",
    "pg_amop",
    "pg_amproc",
    "pg_attrdef",
    "pg_attribute",
    "pg_authid",
    "pg_auth_members",
    "pg_autovacuum",
    "pg_cast",
    "pg_class",
    "pg_constraint",
    "pg_conversion",
    "pg_database",
    "pg_depend",
    "pg_description",
    "pg_index",
    "pg_inherits",
    "pg_language",
    "pg_largeobject",
    "pg_listener",
    "pg_namespace",
    "pg_opclass",
    "pg_operator",
    "pg_pltemplate",
    "pg_proc",
    "pg_rewrite",
    "pg_shdepend",
    "pg_statistic",
    "pg_tablespace",
    "pg_trigger",
    "pg_type",
    "pg_group",
    "pg_indexes",
    "pg_locks",
    "pg_prepared_xacts",
    "pg_roles",
    "pg_rules",
    "pg_settings",
    "pg_shadow",
    "pg_stats",
    "pg_tables",
    "pg_user",
    "pg_views", */
    NULL 
};

typedef enum {
    E_SELECT=0,  /* default 0 ,it's 1 has a job, 2 working */   
    E_INSERT,
    E_UPDATE,
    E_DELETE,
    E_CACHE,
    E_OTHER
} E_SQL_TYPE;

#ifdef __cplusplus
 }
#endif
#endif /* --- #PG_SYSTEM_ATALOGS_H ---*/
 /* vim: set ts=4 sw=4: */

