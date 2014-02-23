/*
 * Author: vyouzhi <vyouzhi@163.com>
 * http://www.xda.cn
 *
 * File: libevent_lib.c
 * Create Date: 2011-09-22 09:29:21
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
#include "action_lib.h"

void on_accept(int fd, short ev, void *arg){
	int client_fd, err_len;
    char *err, *err_log;
	struct sockaddr_in client_addr;
	socklen_t client_len;
    client_len = sizeof(client_addr);
    uint64_t u;
    ssize_t s;
    extern NOTIFY_TOKEN_STATE notify_token_thread;
    extern int token_efd;
    int isDep;

	/* Accept the new connection. */
	client_fd = accept(fd, (struct sockaddr *)&client_addr, &client_len);
	if (client_fd == -1) {
          err = strerror(errno);
        /*err_len = strlen(err);
        err_log = calloc(1, err_len+10);
        sprintf(err_log, "accept-- %s", err);
		  d_log(err_log);
        free(err_log);*/
        DEBUG("accept error %s --", err);
		return;
	}
    if(rq_push(client_fd) != 0){
        RQ_BUSY(isDep);           
        DEBUG("action isDep:%d", isDep);
        close(client_fd);
        d_log("RQ is full");
    }
    if(notify_token_thread == NT_FREE){
        /*  thread = work_threads+1;
          write(thread->notify_write_fd, "", 1);       
             token_sem_post();*/
         u = 1;
         s = write(token_efd, &u , sizeof(uint64_t));

         if(s != sizeof(uint64_t))DEBUG("write to token_efd");

    }

    if(proc_status == NT_FREE){
        proc_status = NT_HAS;
        anyThread(fproc, NULL);
    }

    
}

void conn_new(int sfd, struct event_base *base){
	conns = (conn *)calloc(1, sizeof(conn));
	conns->sfd = sfd;

	event_set(&conns->event, sfd, EV_READ|EV_PERSIST, on_accept, (void *)conns);
	event_base_set(base, &conns->event);
	event_add(&conns->event, 0);

}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  anyThread
 *  Description:  
 * =====================================================================================
 */
void anyThread (void *(*func)(void *), void *arg  ){
    pthread_t       thread;
    pthread_attr_t  attr;
    int ret;
    pthread_attr_init(&attr);
   
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED); 
    if ((ret = pthread_create(&thread, &attr, func, arg)) != 0) {
        d_log("Can't create thread");
        //exit(1);
    }   
    pthread_attr_destroy(&attr);

}		/* -----  end of function anyThread  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  fproc
 *  Description:  
 * =====================================================================================
 */
void *fproc ( void *arg){
    proc_status = NT_WORKING;
    hproc();
    proc_status = NT_FREE;
}		/* -----  end of function fproc  ----- */

/* vim: set ts=4 sw=4: */

