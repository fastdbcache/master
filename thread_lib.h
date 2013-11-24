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
    struct event notify_event;  /* event */
    int notify_read_fd;    /*   pipe */
    int notify_write_fd;     /*  pipe   */
    int socket_fd;
    ssize_t no;
    int master_efd;   /* call master fd*/
} LIBEVENT_WORK_PROCESS;

// http://stackoverflow.com/questions/2289852/how-to-share-a-linked-list-between-two-processes
typedef struct work_process_token WPT;
struct work_process_token{
    int token;
    ssize_t control_token_fail;
    int token_read_fd;
};

typedef struct work_process_queue WPQ;
struct work_process_queue{
    ssize_t no;
    pid_t pid;        /* work pid ID */
    RING_JOB_STATE isjob;
};

typedef struct conn conn;
struct conn {
    int    sfd;
    struct event event;
    /*conn   *next;      Used for generating a list of conn structures 
 *     LIBEVENT_THREAD *thread;  Pointer to the thread object serving this connection */
};

conn *conns;

int share_mem_token; 
int share_mem_wpq;
int process_num;
int master_efd;

struct event_base *main_base; /* main process base */

/* worker thread */
LIBEVENT_WORK_PROCESS *work_process;  /* work threads */

/* child worker process for token thread  */
void work_process_init(int nprocess, int socket_fd, int ev_fd);
void setup_process(LIBEVENT_WORK_PROCESS *me) ;
void worker_libevent(LIBEVENT_WORK_PROCESS *arg);
void libevent_work_process(int fd, short ev, void *arg);
/* work thread end */


#endif /* THREAD_LIB_H_ */
/* vim: set ts=4 sw=4: */

