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
#include "config_global.h"
#include "./modules/modules.h"
#include "./hashtable/pool_init.h"
#include "./deposit/dep_struct.h"

int main(int argc, char* argv[]){
    int listen_fd, unix_sock;
	int h;
	char *c, pid_file[500];
	ssize_t do_daemonize = 0;
    char help[]=" -c etc/suning.cnf\n \
                Usage: \n \
                -c which loading suning.cnf \n \
                -d daemon run \n";


    set_signal();

    if(argc < 2){
        printf("%s%s\n", argv[0], help);
        exit(-1);
    }
    c = NULL;
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

    conn_init_global();

    if(c){
	    conf_init(c);
        conn_get_global();
    }
    
    
	d_log(conn_global->fdbc);

    if(conn_global->hasdep == H_TRUE)
        leadinit(conn_global->dmaxbytes);

    hcreate(8);
    /*rq_init(MAXCONN);*/
    rq_init(256);

    work_thread_init(conn_global->process_num);

    token_thread_init();
    notify_token_thread = NT_FREE;

    main_base = event_init();

	if(do_daemonize == 1 || conn_global->do_daemonize == 1){
		daemon_init(argv[0], 0);	
		if(strlen(conn_global->pid_file) > 0)strcpy(pid_file, conn_global->pid_file);
		else strcpy(pid_file, "/var/run/strawberry.pid");

		save_pid(getpid(), pid_file);
	}

    /* register moduels 
	modules_register();*/

    unix_sock = conn_global->unix_sock;
	if(0 == unix_sock)
		listen_fd = Socket_Init();
	else
		listen_fd = server_socket_unix(); 

	if (listen_fd < 0){
		perror( "listen failed");
		exit(-1);
	}
    
    conn_new(listen_fd, main_base);

    event_base_loop(main_base, 0);

    return 0;
}

/* vim: set ts=4 sw=4: */

