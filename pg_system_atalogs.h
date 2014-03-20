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
#ifndef PG_SYSTEM_ATALOGS_H
#define PG_SYSTEM_ATALOGS_H
#ifdef __cplusplus
 extern "C"
 {
#endif

static const char *Query_for_list[] = {
    "pg_",
    "INFORMATION_SCHEMA",
    "information",
   /*  {12, "pg_aggregate"},
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
    E_CACHE_ITEM,
    E_CACHE_VERSION,
    E_CACHE_STAT,
    E_CACHE_HSTAT,
    E_CACHE_LASTERR,
    E_CACHE_HELP,
    E_CACHE_SET,
    E_CACHE_SET_MAXBYTE,
    E_CACHE_SET_ITEM,
    E_OTHER
} E_SQL_TYPE;


#ifdef __cplusplus
 }
#endif
#endif /* --- #PG_SYSTEM_ATALOGS_H ---*/
 /* vim: set ts=4 sw=4: */

