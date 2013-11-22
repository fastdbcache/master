/*
 * Author: vyouzhi <vyouzhi@163.com>
 * http://www.xda.cn
 *
 * File: libevent_lib.c
 * Create Date: 2011-09-22 09:29:21
 *
 */

#include "action_lib.h"
#include "log_lib.h"
#include "conf_lib.h"

void conn_new(int sfd, struct event_base *base){
	conns = (conn *)calloc(1, sizeof(conn));
	conns->sfd = sfd;

	event_set(&conns->event, 0, EV_READ|EV_PERSIST, NULL, (void *)conns);
	event_base_set(base, &conns->event);
	event_add(&conns->event, 0);

}

/* vim: set ts=4 sw=4: */

