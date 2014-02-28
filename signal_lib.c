/*
 * Author: vyouzhi <vyouzhi@163.com>
 *
 * File: signal_lib.c
 * Create Date: 2011-09-26 15:57:16
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

#include "signal_lib.h"


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  sig_handler
 *  Description:  
 * =====================================================================================
 */

void sig_handler(const int sig){
    exit(-1);
	return;
}		/* -----  end of function sig_handler  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  set_signal
 *  Description:  
 * =====================================================================================
 */

void set_signal(void){
	
	signal(SIGINT, sig_handler);
	signal(SIGTERM, sig_handler);
	signal(SIGQUIT, sig_handler);
	signal(SIGKILL, sig_handler);
	signal(SIGHUP, sig_handler);
    signal(SIGPIPE, SIG_IGN);
	return;
}		/* -----  end of function set_signal  ----- */

/* vim: set ts=4 sw=4: */

