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

void do_dispatch(int fd, short ev, void *arg){
    WPQ *_wpq;
    WPT *_wpt;
    int icount, m, start, _token;
    extern int process_num;
    extern LIBEVENT_WORK_PROCESS *work_process;
    uint64_t u;
    ssize_t s;

    s = read(fd, &u, sizeof(uint64_t));
    if (s != sizeof(uint64_t))
        printf("read err\n");
    _wpq =  (WPQ *)shmat(share_mem_wpq, NULL, 0);
    /* icount = m = 0; 
    do {
        if(_wpq[m].isjob == JOB_FREE)
            icount++;
        m++;
    } while ( m < process_num );				 -----  end do-while  ----- */
    //printf("free wpq is:%d\n", icount);

    _wpt = (WPT *)shmat(share_mem_token, NULL, 0);
    start = _wpt->token;
    do {
        _token = (_wpt->token + 1) % process_num;
        _wpt->token = _wpq[_token].no;
        if(_wpq[_token].isjob == JOB_FREE){
            
            if(write(work_process[_wpq[_token].no].notify_write_fd, "" , 1) != 1){
                printf("master write work process error\n");
            }else{
                _wpt->control_token_fail = 0;       
                break;
            }
        }
    } while ( _token != start );				/* -----  end do-while  ----- */
    //printf("master control\n");
}

void conn_new(int sfd, struct event_base *base){
	conns = (conn *)calloc(1, sizeof(conn));
	conns->sfd = sfd;

	event_set(&conns->event, sfd, EV_READ|EV_PERSIST, do_dispatch, (void *)conns);
	event_base_set(base, &conns->event);
	event_add(&conns->event, 0);

}

/* vim: set ts=4 sw=4: */

