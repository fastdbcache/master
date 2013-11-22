/*
 * Author: vyouzhi <vyouzhi@163.com>
 * http://www.xda.cn
 *
 * File: SIGNAL_LIB
 * Create Date: 2011-09-26 15:57:19
 *
 */
#ifndef SIGNAL_LIB_H_
#define SIGNAL_LIB_H_

#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

void sig_handler(const int sig);
void set_signal(void);
#endif /* SIGNAL_LIB_H_ */
/* vim: set ts=4 sw=4: */

