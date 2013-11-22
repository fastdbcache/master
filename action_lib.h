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

void conn_new(int sfd, struct event_base *base);
#endif /* LIBEVENT_LIB_H_ */
/* vim: set ts=4 sw=4: */

