/*
 * Author: vyouzhi <vyouzhi@163.com>
 * http://www.xda.cn
 *
 * File: thread_lib.c
 * Create Date: 2011-09-26 15:07:25
 *
 */

#include "thread_lib.h"
#include "command.h"
#include "func.h"
#include "log_lib.h"
#include "error_lib.h"
#include "parse_cmd.h"
#include "time_lib.h"

void libevent_work_process(int fd, short ev, void *arg){
    LIBEVENT_WORK_PROCESS *me =(LIBEVENT_WORK_PROCESS *)arg;
    char *start_pack;
    WPQ *_wpq; 
    int pg_fds, m, pg_len, pack_len, _token;
    int frontend, err_len;
    char *err, *err_log;
	struct sockaddr_in frontend_addr;
	socklen_t frontend_len;
    SESSION_SLOTS *_slot;

    
    if(work_process_job != JOB_FREE) goto err;
    
    _wpq = (WPQ *)shmat(share_mem_wpq, NULL, 0);
    _token = (_wpq->token + 1) % process_num;
    if(me->no == _token){
        _wpq->token = me->no;
    }else goto err;

    work_process_job = JOB_HAS;
    start_pack = NULL;
    frontend_len = sizeof(frontend_addr); 
    frontend = accept(me->socket_fd, (struct sockaddr *)&frontend_addr, &frontend_len);
	if (frontend == -1) {
        /*  err = strerror(errno);
        err_len = strlen(err);
        err_log = calloc(1, err_len+10);
        sprintf(err_log, "accept-- %s", err);
		d_log(err_log);
        free(err_log);*/
        printf("0.work process accepet error\n");
		goto ok;
	}
    start_pack = (char *)calloc(1, sizeof(uint32));
    pack_len = PGStartupPacket3(frontend, start_pack);  /* 1. F -> B */
   
    _slot = resolve_slot(start_pack);
    if(_slot != NULL) 
        printf("user:%s, database:%s, major:%d, app:%s\n", _slot->user, _slot->database, _slot->major, _slot->application_name); 

    if(_slot->backend_fd == 0)
        pg_fds = Client_Init(conf_get("pg_host"), atoi(conf_get("pg_port")));
        if(pg_fds == -1)goto bad;
        pg_len = Socket_Send(pg_fds, start_pack, pack_len);
        if(pg_len != pack_len) goto bad;

        if(AuthPG(pg_fds, frontend, _slot)==-1){
            printf("auth error\n");
            goto bad;
        }
    }
        
    pg_len = PGExchange(pg_fds, frontend, _slot);      /*  exchange --- */

    if(pg_len == -1){
        //close(pg_fds);
        goto ok;
    }      
   
    bad:
        free(_slot->user);
        free(_slot->database);
        free(_slot);

    ok:
        if(start_pack != NULL)free(start_pack);
        start_pack = NULL;
        close(frontend); 
        work_process_job = JOB_FREE;
    err:
        (void)0;       
                 
}

void work_process_init(int nprocess, int socket_fd){
    int i;
    WPQ *_wpq;

    work_process = (LIBEVENT_WORK_PROCESS *)calloc(nprocess, sizeof(LIBEVENT_WORK_PROCESS));
    if(!work_process){
        exit(1);
    }
    _wpq = (WPQ *)shmat(share_mem_wpq, NULL, 0);
    _wpq->token = nprocess - 1;
    work_process_job = JOB_FREE;

    for(i=0; i< nprocess; i++){
        pid_t childpid;
       
        work_process[i].socket_fd = socket_fd;
        childpid = fork();
        switch ( childpid ) {
            case -1:	
                printf("Bad fork()\n"); exit(1);
                break;

            case 0:	

                setup_process(&work_process[i]);
                worker_libevent(&work_process[i]);
                break;

            default:	
                break;
        }				/* -----  end switch  ----- */
    }

}

void setup_process(LIBEVENT_WORK_PROCESS *me) {
    me->base = event_init();
    if (! me->base) {
        d_log("error Can't allocate event base");
        exit(1);
    }

    /* Listen for notifications from other threads */
    event_set(&me->notify_event, me->socket_fd,
              EV_READ | EV_PERSIST, libevent_work_process, me);
    event_base_set(me->base, &me->notify_event);

    if (event_add(&me->notify_event, 0) == -1) {
        d_log("Can't monitor libevent notify pipe");
        exit(1);
    }
}

void worker_libevent(LIBEVENT_WORK_PROCESS *arg) {
   
    LIBEVENT_WORK_PROCESS *me = arg;

    event_base_loop(me->base, 0);
}
/* work threads end */



/* vim: set ts=4 sw=4: */

