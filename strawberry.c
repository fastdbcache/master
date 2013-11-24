/*
 * Author: vyouzhi <vyouzhi@163.com>
 * http://www.xda.cn
 *
 * File: snooker.c
 * Create Date: 2012-08-03 14:16:16
 *
 */
#include <error.h>

#include "log_lib.h"
#include "thread_lib.h"
#include "conf_lib.h"
#include "action_lib.h"
#include "daemon_lib.h"
#include "signal_lib.h"
#include "./modules/modules.h"

int main(int argc, char* argv[]){
    int listen_fd, unix_sock;
    pid_t pid;
    int fds[2];
	int h, max_ring_queue;
	char *c, pid_file[500], buf[1];
	ssize_t do_daemonize = 0;
    extern int process_num;
    char help[]=" -c etc/suning.cnf\n \
                Usage: \n \
                -c which loading suning.cnf \n \
                -d daemon run \n";


    set_signal();

    if(argc < 2){
        printf("%s%s\n", argv[0], help);
        exit(-1);
    }

	while((h = getopt(argc, argv, "c:d")) != -1){
		switch(h){
			case 'c':
				c = optarg;
				break;
			case 'd':
				do_daemonize = 1;
				break;
			default:
				printf("%s --help\n", argv[0]);
				exit(-1);
		}
	}

	conf_init(c);

	d_log("snooker:0.0.1 start ...");

	if(do_daemonize == 1 || atoi(conf_get("do_daemonize")) == 1){
		daemon_init(argv[0], 0);	
		if(strlen(conf_get("pid_file")) > 0)strcpy(pid_file, conf_get("pid_file"));
		else strcpy(pid_file, "/var/run/strawberry.pid");

		save_pid(getpid(), pid_file);
	}

    /* register moduels 
	modules_register();*/

    unix_sock = atoi(conf_get("unix_sock"));
	if(0 == unix_sock)
		listen_fd = Socket_Init();
	else
		listen_fd = server_socket_unix(); 

	if (listen_fd < 0){
		perror( "listen failed");
		exit(-1);
	}

    /*if(-1 == SetNonBlocking(listen_fd)){
        printf("noblock error\n");
        close(listen_fd);
        return (-1);
    }*/

    process_num = atoi(conf_get("process_num"));
    if(process_num < 1) exit(-1);
    
    share_mem_wpq = shmget(IPC_PRIVATE, process_num*sizeof(WPQ), SHM_R | SHM_W);
    if(share_mem_wpq == -1 ) exit(-1);

    share_mem_token = shmget(IPC_PRIVATE, sizeof(WPT), SHM_R | SHM_W);
    if(share_mem_token == -1) exit(-1);

    master_efd = eventfd(0, 0); 
    if(master_efd == -1) exit(1);    

    work_process_init(process_num, listen_fd, master_efd);
    
    write(work_process[0].notify_write_fd, "" , 1); 
    main_base = event_init();
	conn_new(master_efd, main_base);
    event_base_loop(main_base, 0);

    return 0;
}

/* vim: set ts=4 sw=4: */

