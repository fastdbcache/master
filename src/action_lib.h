/*
 * Author: vyouzhi <vyouzhi@163.com>
 * http://www.xda.cn
 *
 * File: LIBEVENT_LIB_H
 * Create Date: 2011-09-22 09:29:25
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
#ifndef LIBEVENT_LIB_H_
#define LIBEVENT_LIB_H_

#include "thread_lib.h"
#include "./hashtable/pool_proc.h"
#include "./deposit/dep_struct.h"

NOTIFY_TOKEN_STATE proc_status;

void do_dispatch(int fd, short ev, void *arg);
void conn_new(int sfd, struct event_base *base);
void anyThread (void *(*func)(void *), void *arg  );
void *fproc ( void *arg );
#endif /* LIBEVENT_LIB_H_ */
/* vim: set ts=4 sw=4: */

