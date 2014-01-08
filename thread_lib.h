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

#include "socket_lib.h"
#include "pg_protocol.h"

#define MAX_BACKEND_SESSION 128

#define ERROR_BREAK(client_sfd, send_val, send_val_len, type_val) do {	\
    (type_val) = 0;                                                \
    Socket_Send((client_sfd), (send_val), (send_val_len)); \
} while (0)

#define FIND_RQ(cq, stat) do{\
    FIND_Q(RQ, cq, isjob, stat); \
}while(0)

#define FIND_WTQ(cq, stat) do{\
    FIND_Q(WTQ, cq, isjob, stat); \
}while(0)

#define FIND_WTQPID(cq, stat) do{\
    FIND_Q(WTQ, cq, pid, stat); \
}while(0)

#define FIND_Q(type, cq, table, stat) do{\
    type *rq_head = (cq);             \
    do{                             \
        if(rq_head->table==(stat)) break; \
        rq_head = rq_head->next;            \
    }while(rq_head!=(cq));                  \
    (cq) = rq_head;                       \
}while(0)

#define RQ_BUSY(c)  do{    \
    RQ_COUNT((c));               \
    (c) = MAXCONN - (c);             \
}while(0)

#define RQ_FREE(c)  do{    \
    RQ_COUNT((c));               \
}while(0)

#define RQ_COUNT(c)  do{    \
    (c) = 0;                \
    RQ *_trq = rq_queue_tail;    \
    while(_trq!=rq_queue_head){   \
        if(_trq->isjob == JOB_FREE) \
            (c)++;               \
        _trq = _trq->next;          \
    }                           \
}while(0)

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


/* main thread call it , and get one rq to child thread*/
typedef struct {
    pthread_t thread_id;        /* token thread ID */
    struct event_base *base;    /* libevent headle */
    struct event notify_event;  /* 通知事件，主线程通过这个事件通知worker线程有新连接 */

} LIBEVENT_TOKEN_THREAD;

/* token thread call it , and get one rq to work thread*/
typedef struct {
    pthread_t thread_id;        /* token thread ID */
    struct event_base *base;    /* libevent headle */
    struct event notify_event;  /* 通知事件，主线程通过这个事件通知worker线程有新连接 */
    int notify_read_fd;    /*   通知事件关联的读fd，这和下面的notify_send_fd是一对管道，具体使用后面讲 */
    int notify_write_fd;     /*     通知事件关联的写fd，后面讲 */
    int no;
    
} LIBEVENT_WORK_THREAD;

/* a client item  */
typedef struct frontend_conn_item FC_ITEM;
struct frontend_conn_item{
    int ffd;    /* frontend connect fd */
    double ctime;  /* start time and for overtime */ 
};

/* a ring connect queue  */
typedef struct ring_queue RQ;
struct ring_queue{
    FC_ITEM *frontend; 
    RING_JOB_STATE isjob;
    RQ *next; 
};

typedef struct work_thread_queue WTQ;
struct work_thread_queue{
    RING_JOB_STATE isjob; 
    pthread_t pid;
    int no;
    RQ *rq_item;
    WTQ *next;
};

typedef struct conn conn;
struct conn {
    int    sfd;
    struct event event;
    /*conn   *next;      Used for generating a list of conn structures 
 *     LIBEVENT_THREAD *thread;  Pointer to the thread object serving this connection */
};

conn *conns;

RQ *rq_queue_head;
RQ *rq_queue_tail;

WTQ *wtq_queue_head;
WTQ *wtq_queue_tail;

struct event_base *main_base; /* main process base */

LIBEVENT_TOKEN_THREAD *token_thread; /* manager from main thread job, and chose one free child thread do work */
NOTIFY_TOKEN_STATE notify_token_thread; /* wait for child notify thread  */

/* worker thread */
LIBEVENT_WORK_THREAD *work_threads;  /* work threads */
pthread_mutex_t init_work_lock;
pthread_cond_t init_work_cond;


int token_efd;
/*sem_t token_sem;        synch semaphore         */

/* child worker thread for token process  */
void work_thread_init(int nthreads);
void setup_thread(LIBEVENT_WORK_THREAD *me) ;
void create_worker(void *(*func)(void *), void *arg);
void *worker_libevent(void *arg);
void libevent_work_thread(int fd, short ev, void *arg);
/* work thread end */

/* child notify thread for main process */

void token_thread_init();
void setup_token(void *(*func)(void *), void *arg);
void *main_token_thread(void *arg);
void libevent_token_thread( int fd, short ev,void *arg);
/* token thread end */


/* RQ  */
RQ *rq_item_init();
int rq_init(int numbers);
int rq_push(int client_fd);
RQ *rq_pop();
void rq_free(RQ *child);

/* WTQ */
WTQ *wtq_init();
void wtq_push();
WTQ *wtq_pop();

#endif /* THREAD_LIB_H_ */
/* vim: set ts=4 sw=4: */

