/*
 *   Author:
 *   vyouzhi <vyouzhi@163.com>
 *   http://www.xda.cn
 *
 *   File:                    DAEMON_LIB
 *   Create Date:      2011-10-10 00:01:54
 */

#ifndef DAEMON_LIB_H_     
#define DAEMON_LIB_H_

#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <error.h>
#include "log_lib.h"

int daemon_init(const char *pname, int facility);
void save_pid(const pid_t pid, const char *pid_file);
void remove_pidfile(const char *pid_file);
#endif /* DAEMON_LIB_H */
/* vim: set ts=4 sw=4: */

