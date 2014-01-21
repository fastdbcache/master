/*
 * Author: vyouzhi <vyouzhi@163.com>
 * http://www.xda.cn
 *
 * File: signal_lib.c
 * Create Date: 2011-09-26 15:57:16
 *
 */


#include "signal_lib.h"

void sig_handler(const int sig){
    exit(-1);
	return;
}

/* Set signal handler */
void set_signal(void){
	
	signal(SIGINT, sig_handler);
	signal(SIGTERM, sig_handler);
	signal(SIGQUIT, sig_handler);
	signal(SIGKILL, sig_handler);
	signal(SIGHUP, sig_handler);
    signal(SIGPIPE, SIG_IGN);
	return;
}
/* vim: set ts=4 sw=4: */

