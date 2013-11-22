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

RING_JOB_STATE work_process_job;
void libevent_work_process(int fd, short ev, void *arg){
    LIBEVENT_WORK_PROCESS *me =(LIBEVENT_WORK_PROCESS *)arg;
    PACK *start_pack;
    WPQ *_wpq, *_wpq_next, *_wpq_me; 
    WPT *_wpt;
    int pg_fds, m, pg_len, pack_len, _token;
    int frontend, err_len;
    char *err, *err_log;
	struct sockaddr_in frontend_addr;
	socklen_t frontend_len;
    SESSION_SLOTS *_slot;


    if(work_process_job != JOB_FREE) goto err;
    work_process_job = JOB_HAS;
    /*  _wpq = (WPQ *)shmat(share_mem_wpq, NULL, 0);
    if(_wpq == NULL){
        printf("1._wpq is null\n");
        goto err;
    }
    _wpq_me = _wpq + me->no;
    if(_wpq_me == NULL) {printf("2._wpq_me is null\n");goto err;}

    if(_wpq_me->isjob != JOB_FREE) {printf("3.job no free:%d \n", _wpq_me->no);goto err;}
    _wpq_me->isjob = JOB_HAS;

    _wpt = (WPT *)shmat(share_mem_token, NULL, 0);
      
    if ( _wpt == NULL ) {
        printf("4._wpt null\n");
        goto err;
    }
    _token = (_wpt->token + 1) % process_num;
    //printf("5.now _token:%d, no:%d\n", _token, _wpq_me->no);
     
    //if(_wpq_me->no != _token) goto err;*/

    frontend_len = sizeof(frontend_addr); 
    frontend = accept(me->socket_fd, (struct sockaddr *)&frontend_addr, &frontend_len);
    if (frontend == -1) {
        /*  err = strerror(errno);
        err_len = strlen(err);
        err_log = calloc(1, err_len+10);
        sprintf(err_log, "accept-- %s", err);
        d_log(err_log);
        free(err_log);*/
        printf("0.work process accepet error no: %d\n", _wpq_me->no);
        goto err;
    }

   // printf("6._wpq_me no:%d\n", _wpq_me->no);
    
    start_pack = (PACK *)calloc(1, sizeof(PACK));
    start_pack->pack = (char *)calloc(1, sizeof(char));
    pack_len = PGStartupPacket3(frontend, start_pack);  /* 1. F -> B */
    if(pack_len == -1)goto ok;
     
    _slot = resolve_slot(start_pack->pack);
    /*  if(_slot != NULL) 
        printf("user:%s, database:%s, major:%d, app:%s, pid:%llu\n", _slot->user, _slot->database, _slot->major, _slot->application_name, getpid()); 
    */
    if(_slot->backend_fd == 0){
        pg_fds = Client_Init(conf_get("pg_host"), atoi(conf_get("pg_port")));
        if(pg_fds == -1){
            if(start_pack->pack != NULL)free(start_pack->pack);
            start_pack->pack = NULL;
            goto bad;
        }
        pg_len = Socket_Send(pg_fds, start_pack->pack, pack_len);
         
        
        if(pg_len != pack_len) goto bad;
        if(AuthPG(pg_fds, frontend, _slot)==-1){
            printf("auth error\n");
            goto bad;
        }
    }
        
    pg_len = PGExchange(pg_fds, frontend, _slot);      /*  exchange --- */
    if(pg_len == -1){
        //close(pg_fds);
        goto bad;
    }      
   
    bad:
        free(_slot->user);
        free(_slot->database);
        free(_slot);

    ok:
        if(start_pack->pack){
            free(start_pack->pack);
        }
        start_pack->pack = NULL;
        close(pg_fds);
        close(frontend); 
       // _wpq_me->isjob = JOB_FREE;
        work_process_job = JOB_FREE;

    err:
        
        (void)0;       
                 
}

void work_process_init(int nprocess, int socket_fd){
    int i;
    WPQ *_wpq, *_wpq_me;
    WPT *_wpt;

    work_process = (LIBEVENT_WORK_PROCESS *)calloc(nprocess, sizeof(LIBEVENT_WORK_PROCESS));
    if(!work_process){
        printf("no process create\n");
        exit(1);
    }
    work_process_job = JOB_FREE;
    
    _wpt = (WPT *)shmat(share_mem_token, NULL, 0);
    _wpt->token = nprocess - 1;

    for(i=0; i< nprocess; i++){
        pid_t childpid;
       
        work_process[i].socket_fd = socket_fd;
        childpid = fork();
        switch ( childpid ) {
            case -1:	
                printf("Bad fork()\n"); exit(1);
                break;

            case 0:	
                _wpq = (WPQ *)shmat(share_mem_wpq, NULL, 0);
                if(_wpq == NULL) {
                    printf("wpq is null\n");
                    exit(1);
                }
                _wpq_me = _wpq+i;
                if(_wpq_me == NULL) {
                    printf("_wpq_me is null\n");
                    exit(1);
                }

                _wpq_me->no = i; 
                _wpq_me->isjob = JOB_FREE;
                _wpq_me->pid = getpid();

                work_process[i].pid = _wpq_me->pid;
                work_process[i].no = i;

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

