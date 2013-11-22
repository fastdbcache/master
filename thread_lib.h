/*
 * Author: vyouzhi <vyouzhi@163.com>
 * http://www.xda.cn
 *
 * File: THREAD_LIB.H
 * Create Date: 2011-09-26 15:07:44
 *
 */
#ifndef THREAD_LIB_H_
#define THREAD_LIB_H_

#include <event.h>
#include <pthread.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <sys/eventfd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "socket_lib.h"
#include "pg_protocol.h"

#define MAX_BACKEND_SESSION 128

#define ERROR_BREAK(client_sfd, send_val, send_val_len, type_val) do {	\
    (type_val) = 0;                                                \
    Socket_Send((client_sfd), (send_val), (send_val_len)); \
} while (0)

typedef enum {
    JOB_FREE=0,  /* default 0 ,it's 1 has a job, 2 working */
    JOB_HAS,
    JOB_WORKING
} RING_JOB_STATE;

typedef enum {
    NT_FREE=0,  /* default 0 ,it's 1 has a job, 2 working */
    NT_HAS,
    NT_WORKING
} NOTIFY_TOKEN_STATE;

/* main process call it , and get one rq to work process*/
typedef struct {
    pid_t pid;        /* work pid ID */
    struct event_base *base;    /* libevent headle */
    struct event notify_event;  /* 通知事件，主线程通过这个事件通知worker线程有新连接 */
    int notify_read_fd;    /*   通知事件关联的读fd，这和下面的notify_send_fd是一对管道，具体使用后面讲 */
    int notify_write_fd;     /*     通知事件关联的写fd，后面讲 */
    int no;
    int socket_fd; 
} LIBEVENT_WORK_PROCESS;
// http://stackoverflow.com/questions/2289852/how-to-share-a-linked-list-between-two-processes
typedef struct work_process_queue WPQ;
struct work_process_queue{
    int token;
};

typedef struct conn conn;
struct conn {
    int    sfd;
    struct event event;
    /*conn   *next;      Used for generating a list of conn structures 
 *     LIBEVENT_THREAD *thread;  Pointer to the thread object serving this connection */
};

conn *conns;

int share_mem_wpq;
RING_JOB_STATE work_process_job;
int process_num;

struct event_base *main_base; /* main process base */

/* worker thread */
LIBEVENT_WORK_PROCESS *work_process;  /* work threads */

/* child worker process for token thread  */
void work_process_init(int nprocess, int socket_fd);
void setup_process(LIBEVENT_WORK_PROCESS *me) ;
void worker_libevent(LIBEVENT_WORK_PROCESS *arg);
void libevent_work_process(int fd, short ev, void *arg);
/* work thread end */


#endif /* THREAD_LIB_H_ */
/* vim: set ts=4 sw=4: */

