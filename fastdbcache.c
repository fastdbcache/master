/*
 * Author: vyouzhi <vyouzhi@163.com>
 * 
 *
 * File: fastdbcache.c
 * Create Date: 2012-08-03 14:16:16
 *
 */
/*    Copyright 2009 10gen Inc.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
#include <error.h>
#include <limits.h>
#include <sys/resource.h>

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
    struct rlimit rlim;
	ssize_t do_daemonize = 0;
    char help[]="%s -c /usr/local/etc/fdbc.conf\n \
                Usage: \n \
                -c which loading fdbc.conf \n \
                -d daemon run \n";


    set_signal();

    if(argc < 2){
        printf(help, argv[0]);
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
    
    pathCheck ( );
	d_log(conn_global->fdbc);

    pools_hfd = inithfd();
    hcreate(8);

    if(conn_global->hasdep == H_TRUE)
        leadinit(conn_global->dmaxbytes);

	if(do_daemonize == 1 || conn_global->do_daemonize == 1){
		daemon_init(argv[0], 0);	
		if(strlen(conn_global->pid_file) > 0)strcpy(pid_file, conn_global->pid_file);
		else strcpy(pid_file, "/var/run/fdbcd.pid");

		save_pid(getpid(), pid_file);
	}
    
    if (getrlimit(RLIMIT_NOFILE, &rlim) != 0) { 
        DEBUG("failed to getrlimit number of files");
        exit(1);
    } else {
        rlim.rlim_cur = conn_global->maxconns;
        rlim.rlim_max = conn_global->maxconns;
        if (setrlimit(RLIMIT_NOFILE, &rlim) != 0) { 
            DEBUG("failed to set rlimit for open files. Try starting as root or requesting smaller maxconns value.");
            exit(1);
        }    
    }         

    /*rq_init(MAXCONN);*/
    rq_init(conn_global->maxconn);

    work_thread_init(conn_global->process_num);

    token_thread_init();
    notify_token_thread = NT_FREE;

    pools_qerr = (QERR *)calloc(1, sizeof(QERR));
    pools_qerr->errs = (ERREC *)calloc(ERR_ROW, sizeof(ERREC));

    main_base = event_init();
    /* register moduels 
	modules_register();*/

    unix_sock = conn_global->unix_sock;
	if(0 == unix_sock)
		listen_fd = Socket_Init();
	else
		listen_fd = server_socket_unix(); 

	if (listen_fd < 0){
        DEBUG("listen_fd  error");
		exit(-1);
	}
    
    conn_new(listen_fd, main_base);

    event_base_loop(main_base, 0);

    return 0;
}



/* vim: set ts=4 sw=4: */

