/*
 * Author: vyouzhi <vyouzhi@163.com>
 * http://www.xda.cn
 *
 * File: LIBEVENT_LIB_H
 * Create Date: 2011-09-22 09:29:25
 *
 */
#ifndef LIBEVENT_LIB_H_
#define LIBEVENT_LIB_H_

#include "thread_lib.h"
#include "./hashtable/pool_proc.h"
#include "./deposit/dep_struct.h"

NOTIFY_TOKEN_STATE proc_status;
NOTIFY_TOKEN_STATE depo_status;

void do_dispatch(int fd, short ev, void *arg);
void conn_new(int sfd, struct event_base *base);
void anyThread ( void );
void *fproc ( void *arg );
void *fdepo ( void *arg );
#endif /* LIBEVENT_LIB_H_ */
/* vim: set ts=4 sw=4: */

