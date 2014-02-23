/*
 *   Author:
 *   vyouzhi <vyouzhi@163.com>
 *   http://www.xda.cn
 *
 *   File:                    daemon_lib.c
 *   Create Date:      2011-10-09 23:50:24
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
#include "daemon_lib.h"

#define MAXFD 64


int daemon_init(const char *pname, int facility){
	int i;
	pid_t pid;
	char id[10];
	
	if((pid = fork()) < 0)
		return (-1);
	else if(pid){
        DEBUG("daemon pid error");
		_exit(0);
    }

	if(setsid() < 0)
		return(-1);

	signal(SIGHUP, SIG_IGN);
	
	if((pid = fork()) < 0)
		return (-1);
	else if(pid){
        DEBUG("daemon pid error");
		_exit(0);
    }
	
	chdir("/");

	for(i=0; i<MAXFD; i++)
		close(i);
	sprintf(id, "%d\n", getpid());
	open("/dev/null", O_RDONLY);
	open("/dev/null", O_RDWR);
	open("/dev/null", O_RDWR);
	
	return(0);
}

void save_pid(const pid_t pid, const char *pid_file) {
    FILE *fp;
    if (pid_file == NULL)
        return;

    if ((fp = fopen(pid_file, "w")) == NULL) {
        d_log("Could not open the pid file  for writing");
        return;
    }

    fprintf(fp,"%ld\n", (long)pid);
    if (fclose(fp) == -1) {
        d_log("Could not close the pid file ");
        return;
    }
}

void remove_pidfile(const char *pid_file) {
  if (pid_file == NULL)
      return;

  if (unlink(pid_file) != 0) {
      d_log("Could not remove the pid file ");
  }

}
/* vim: set ts=4 sw=4: */

