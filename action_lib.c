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

void on_accept(int fd, short ev, void *arg){
	int client_fd, err_len;
    char *err, *err_log;
	struct sockaddr_in client_addr;
	socklen_t client_len;
    LIBEVENT_WORK_THREAD *thread;
    client_len = sizeof(client_addr);
    uint64_t u;
    ssize_t s;
    extern NOTIFY_TOKEN_STATE notify_token_thread;
    extern int token_efd;
        
	/* Accept the new connection. */
	client_fd = accept(fd, (struct sockaddr *)&client_addr, &client_len);
	if (client_fd == -1) {
        err = strerror(errno);
        err_len = strlen(err);
        err_log = calloc(1, err_len+10);
        sprintf(err_log, "accept-- %s", err);
		d_log(err_log);
        free(err_log);
		return;
	}
    if(rq_push(client_fd) == 0){
        if(notify_token_thread == NT_FREE){
            /*  thread = work_threads+1;
              write(thread->notify_write_fd, "", 1);       
                 token_sem_post();*/
             u = 1;
             s = write(token_efd, &u , sizeof(uint64_t));

             if(s != sizeof(uint64_t))printf("error---\n");
 
        }else{
            printf("accept error\n");
        }

        

    }else{
        close(client_fd);
        d_log("RQ is full");
    }

    if(proc_status == NT_FREE){
        procThread();
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
 *         Name:  procThread
 *  Description:  
 * =====================================================================================
 */
void procThread (  ){
    pthread_t       thread;
    pthread_attr_t  attr;
    int ret;
    pthread_attr_init(&attr);
    
    proc_status = NT_HAS;
            
    if ((ret = pthread_create(&thread, &attr, fproc, NULL)) != 0) {
        d_log("Can't create thread");
        printf("here---\n");
        exit(1);
    }   
}		/* -----  end of function procThread  ----- */


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

