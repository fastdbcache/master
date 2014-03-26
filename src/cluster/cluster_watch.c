/*
 * =====================================================================================
 *
 *       Filename:  cluster_watch.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/25/2014 04:33:04 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include "cluster_struct.h"

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  watch_init
 *  Description:  
 * =====================================================================================
 */
void watch_init ( ){

    const char *host = conf_get("server_ip");
	port = atoi(conf_get("server_port"));
	
	return Socket_bind(host, port);   
}		/* -----  end of function watch_init  ----- */

 /* vim: set ts=4 sw=4: */

