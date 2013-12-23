/*
 * Author: vyouzhi <vyouzhi@163.com>
 * http://www.xda.cn
 *
 * File: thread_lib.c
 * Create Date: 2011-09-26 15:07:25
 *
 */

#include "thread_lib.h"
#include "config_global.h"
#include "func.h"
#include "log_lib.h"
#include "error_lib.h"
#include "time_lib.h"

/* work thread start */
int init_count = 0;
void libevent_work_thread(int fd, short ev, void *arg){
    LIBEVENT_WORK_THREAD *me =(LIBEVENT_WORK_THREAD *)arg;
    char buf[1], *pv_buf;
    int ffd;
    WTQ *work_child; 
    SESSION_SLOTS *slot;
    int pg_fds, m, pg_len, client_len, pack_len;

    PACK *start_pack;
    SESSION_SLOTS *_slot;

    if (read(fd, buf, 1) != 1)
	    d_log("error Can't read from libevent pipe");

     
    work_child = wtq_queue_tail;
    wtq_queue_tail = wtq_queue_tail->next;

   // printf("thread_id:%llu\n", me->thread_id);

    FIND_WTQPID(work_child, me->thread_id);
    if(work_child->isjob == JOB_HAS)
        work_child->isjob = JOB_WORKING;
    else{
         goto err;
    }
    //rq_child = child->rqchild;
    //pg_fds = Client_Init(conf_get("pg_host"), atoi(conf_get("pg_port")));
    //cmd = (char *)calloc(1, sizeof(char));
    
    ffd = work_child->rq_item->frontend->ffd;

    start_pack = (PACK *)calloc(1, sizeof(PACK));
    start_pack->pack = (char *)calloc(1, sizeof(char));
    pack_len = PGStartupPacket3(ffd, start_pack);  /*  1. F -> B */
    if(pack_len == -1)goto ok; 

    pg_fds = Client_Init(conf_get("pg_host"), atoi(conf_get("pg_port")));
    if(pg_fds == -1){
        if(start_pack->pack != NULL)free(start_pack->pack);
        start_pack->pack = NULL;
        goto bad;
    }
    pg_len = Socket_Send(pg_fds, start_pack->pack, pack_len);


    if(pg_len != pack_len) goto bad;
    _slot = resolve_slot(start_pack->pack);
    
   if(_slot->backend_fd == 0){
       /*  pg_fds = Client_Init(conf_get("pg_host"), atoi(conf_get("pg_port")));
        if(pg_fds == -1){
            if(start_pack->pack != NULL)free(start_pack->pack);
            start_pack->pack = NULL;
            goto bad;
        }
        pg_len = Socket_Send(pg_fds, start_pack->pack, pack_len);

         */
        if(pg_len != pack_len) goto bad;
        if(AuthPG(pg_fds, ffd, _slot, work_child->no)==-1){
            //printf("auth error\n");
            //            goto bad;
            //                    }else{
            //
            //                            }
            //                                }
            //
            //                                 
            //                    }
        }else{
        }
    }        
    

    work_child->isjob = JOB_FREE;

    //rq_free(rq_child);

    //child->rqchild = NULL;
      
    if(notify_token_thread == NT_FREE){
        uint64_t u;
        ssize_t s;
        u = 1;
        s = write(token_efd, &u , sizeof(uint64_t));
        if(s != sizeof(uint64_t)){
            printf("write error s:%d\n", s);
        }
        //printf("token_efd\n");
    }


    ok:

    bad:

    err:
        close(work_child->rq_item->frontend->ffd);
        work_child->rq_item->isjob = JOB_FREE;
        work_child->isjob = JOB_FREE;
        printf("end\n");
        //rq_free(rq_child);

        //(void)0;

    /* clean_sfd:
        if(Socket_Close(rq_child->client->cfd) == -1) d_log("close fd error!\n");
        rq_child->isjob = JOB_FREE;	
    */
}

void work_thread_init(int nthreads){
    int i;

    work_threads = (LIBEVENT_WORK_THREAD *)calloc(nthreads, sizeof(LIBEVENT_WORK_THREAD));    
    if(!work_threads){
        exit(1);
    }

    wtq_queue_tail = wtq_queue_head = NULL;

    pthread_mutex_init(&init_work_lock, NULL);
    pthread_cond_init(&init_work_cond, NULL);

    for(i=0; i< nthreads; i++){
        int fds[2];
        if(pipe(fds)){

        }
        work_threads[i].notify_read_fd = fds[0];
        work_threads[i].notify_write_fd = fds[1];
        work_threads[i].no = i;
        setup_thread(&work_threads[i]);
    }

    for(i=0; i<nthreads; i++){
        create_worker(worker_libevent, &work_threads[i]);
    }

    pthread_mutex_lock(&init_work_lock);
    while (init_count < nthreads) {
        pthread_cond_wait(&init_work_cond, &init_work_lock);
    }
    wtq_queue_head->next = wtq_queue_tail;
    pthread_mutex_unlock(&init_work_lock);
}

void setup_thread(LIBEVENT_WORK_THREAD *me) {
    me->base = event_init();
    if (! me->base) {
        d_log("error Can't allocate event base");
        exit(1);
    }

    /* Listen for notifications from other threads */
    event_set(&me->notify_event, me->notify_read_fd,
              EV_READ | EV_PERSIST, libevent_work_thread, me);
    event_base_set(me->base, &me->notify_event);

    if (event_add(&me->notify_event, 0) == -1) {
        d_log("Can't monitor libevent notify pipe");
        exit(1);
    }
}

void create_worker(void *(*func)(void *), void *arg) {
    pthread_t       thread;
    pthread_attr_t  attr;
    int ret;
    pthread_attr_init(&attr);
    
    if ((ret = pthread_create(&thread, &attr, func, arg)) != 0) {
        d_log("Can't create thread");
        printf("here---\n");
        exit(1);
    }
}

void *worker_libevent(void *arg) {
    WTQ *work_child;

    LIBEVENT_WORK_THREAD *me = (LIBEVENT_WORK_THREAD *)arg;

    /* Any per-thread setup can happen here; thread_init() will block until
     * all threads have finished initializing.
     */
    
    pthread_mutex_lock(&init_work_lock);
    
    work_child = wtq_init();     
    work_child->pid = pthread_self();
    me->thread_id = work_child->pid;

    work_child->no = me->no;

    if(wtq_queue_head == NULL){
        wtq_queue_tail = wtq_queue_head = work_child;
    }else{
        wtq_queue_head->next = work_child;
        wtq_queue_head = work_child;
    }

    init_count++;
    /*  error_add(pthread_self());*/
    pthread_cond_signal(&init_work_cond);
    pthread_mutex_unlock(&init_work_lock);

    wtq_queue_head->next = wtq_queue_tail;

    event_base_loop(me->base, 0);
}
/* work threads end */

/* token thread start */
void token_thread_init(){

    token_thread = (LIBEVENT_TOKEN_THREAD *)calloc(1, sizeof(LIBEVENT_TOKEN_THREAD));
    token_thread->base = event_base_new();
    if (! token_thread->base) {
        d_log("error Can't allocate event base");
        exit(1);
    }   
    token_efd = eventfd(0, 0);
    if(token_efd == -1) exit(1);

    /* Listen for notifications from other threads */
    event_set(&token_thread->notify_event, token_efd,
              EV_READ | EV_PERSIST, libevent_token_thread, token_thread);
    event_base_set(token_thread->base, &token_thread->notify_event);
    if (event_add(&token_thread->notify_event, 0) == -1) {
        d_log("Can't monitor libevent notify pipe");
        exit(1);
    }   

    setup_token(main_token_thread, token_thread);
}

void setup_token(void *(*func)(void *), void *arg) {
    pthread_t       thread;
    pthread_attr_t  attr;
    int ret;
    pthread_attr_init(&attr);

    if ((ret = pthread_create(&thread, &attr, func, arg)) != 0) {
        d_log("Can't create thread");
        exit(1);
    }
}

void *main_token_thread(void *arg){
    LIBEVENT_TOKEN_THREAD *me = arg;
    event_base_loop(me->base, 0);
}

void libevent_token_thread( int fd, short ev,void *arg){
    uint64_t u;
    ssize_t s;

    s = read(token_efd, &u, sizeof(uint64_t));
    if (s != sizeof(uint64_t)){
        printf("s is error\n");
    }

    notify_token_thread = NT_WORKING;
    do{
        RQ *rq_item = rq_pop();
        if(rq_item == NULL) break;
        if(rq_item->isjob == JOB_HAS) {
            
            WTQ *work_child = wtq_queue_tail;
            
            FIND_WTQ(work_child, JOB_FREE);

            if(work_child->isjob == JOB_FREE){

                rq_item->isjob = JOB_WORKING;
                work_child->rq_item = rq_item;
                work_child->isjob = JOB_HAS;

                LIBEVENT_WORK_THREAD *thread;
                thread = work_threads + work_child->no; 

                if(thread == NULL){
                    printf("thread null \n");
                    rq_item->isjob = JOB_HAS;
                    work_child->isjob = JOB_FREE;
                    break;
                }

                if(write(thread->notify_write_fd, "", 1) != 1){
                    printf("write thread error\n");
                }
            }
        }
        else{
            printf("break\n");
            break;
        }
    }while(1);

    notify_token_thread = NT_FREE;
}
/* token thread end */


/*
 * for init cc
 */
RQ *rq_item_init(){
    RQ *rq;

    rq = (RQ *)calloc(1, sizeof(RQ));
    if(rq == NULL){
        d_log("Can't calloc for RQ");
        exit(1);    
    }   

    rq->frontend = (FC_ITEM *)calloc(1, sizeof(FC_ITEM));
    if(rq->frontend == NULL){
        d_log("Can't calloc for CC_ITEM");
        exit(1);
    }   

    rq->isjob = JOB_FREE;
    rq->next = NULL;

    return rq; 
}

/*
 * for start init rq
 */
int rq_init(int numbers){
    int n;
    RQ *rq_child;

    if(numbers < 1) return -1;
    
    rq_queue_head = rq_item_init();
    rq_queue_tail = rq_queue_head;

    for(n=1; n<numbers; n++){
        rq_child = rq_item_init();
        rq_queue_head->next = rq_child;
        rq_queue_head = rq_child;
    }

    rq_queue_head->next = rq_queue_tail;
    rq_queue_head = rq_queue_tail;

    return 0;
}

/*
 * for main thread
 */
int rq_push(int client_fd){

    FIND_RQ(rq_queue_head, JOB_FREE);
   
    if(rq_queue_head->isjob != JOB_FREE) return -1;

    rq_queue_head->frontend->ffd = client_fd;
    rq_queue_head->frontend->ctime = get_sec() ;
    rq_queue_head->isjob = JOB_HAS;
    rq_queue_head = rq_queue_head->next;

    return 0;
}

/*
 *  for token_process
 */
RQ *rq_pop(){
    RQ *rq_i;
    
    FIND_RQ(rq_queue_tail, JOB_HAS);
    
    if(rq_queue_tail->isjob != JOB_HAS){
        return NULL;
    }
    
    rq_i = rq_queue_tail;
    rq_queue_tail = rq_queue_tail->next;

    return rq_i; 
}

/*
 *  for token_process
 */
void rq_free(RQ *child){

    if(child != NULL){
        close(child->frontend->ffd);
        /*  if(Socket_Close(child->client->cfd) == -1) d_log("close fd error!\n");*/
        child->isjob = JOB_FREE; 
        if(child->frontend != NULL)
            free(child->frontend);
        free(child);
    }
}


/* WTQ */
WTQ *wtq_init(){

    WTQ *work_child;
    work_child = (WTQ *)calloc(1, sizeof(WTQ));
    work_child->rq_item = NULL;
    bzero(work_child->slots, MAX_BACKEND_SESSION);
    work_child->next = NULL;
    
    return work_child;
}

/* vim: set ts=4 sw=4: */

