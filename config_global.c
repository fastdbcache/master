/*
 * =====================================================================================
 *
 *       Filename:  config_global.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/28/2013 10:34:09 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include "config_global.h"

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  conn_init_global
 *  Description:  
 * =====================================================================================
 */
void conn_init_global ( void ){
    conn_global = (_conn *)calloc(1, sizeof(_conn));
    conn_global->server_ip = "localhost";
    conn_global->server_port = 8008;

    conn_global->unix_sock = 0;
    conn_global->path = "/tmp/sqlproxy.sock";

    conn_global->pg_host = "localhost";
    conn_global->pg_port = 5432;

    conn_global->do_daemonize = 0;
    conn_global->pid_file = "/var/run/server.pid";

    conn_global->process_num = 2;

    conn_global->factor = 1.25;
    conn_global->maxbytes = 64 * 1024 * 1024 ;
}		/* -----  end of function conn_init_global  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  conn_get_global
 *  Description:  
 * =====================================================================================
 */
void conn_get_global (  ){

    conn_global->server_ip = conf_get("server_ip");
    conn_global->server_port = atoi(conf_get("server_port"));

    conn_global->unix_sock = atoi(conf_get("unix_sock"));
    conn_global->path = conf_get("path");

    conn_global->pg_host = conf_get("pg_host");
    conn_global->pg_port = atoi(conf_get("pg_port"));

    conn_global->do_daemonize = atoi(conf_get("do_daemonize"));
    conn_global->pid_file = conf_get("pid_file");

    conn_global->process_num = atoi(conf_get("process_num"));

    conn_global->factor = 1.25;
    conn_global->maxbytes = 64 * 1024 * 1024;

    return ;
}		/* -----  end of function conn_get_global  ----- */
 /* vim: set ts=4 sw=4: */

