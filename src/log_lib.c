/*
 * Author: vyouzhi <vyouzhi@163.com>
 *
 * File: log_lib.c
 * Create Date: 2011-10-10 09:37:15
 *
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
#include "log_lib.h"


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  eprintf
 *  Description:  
 * =====================================================================================
 */
void eprintf ( Err_str_t est, const char *format, ... ){
    char buffer[ERR_LENG];
    va_list args;
    va_start (args, format);
    vsnprintf (buffer, ERR_LENG-1, format, args);
    //printf ("eprint:%s: %s\n",est.estr, buffer);
    elog_write(est, buffer);
    va_end (args);    
}		/* -----  end of function eprintf  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  elog_write
 *  Description:  
 * =====================================================================================
 */

void elog_write( Err_str_t est, const char *msg ){
	openlog(LOGFILE, LOG_PID|LOG_CONS, LOG_LOCAL0);
	syslog(LOG_ALERT,"%s:%s",est.estr, msg);
	closelog();
}		/* -----  end of function elog_write  ----- */



/* vim: set ts=4 sw=4: */

