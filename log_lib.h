/*
 * Author: vyouzhi <vyouzhi@163.com>
 * http://www.xda.cn
 *
 * File: LOG_LIG
 * Create Date: 2011-10-10 09:37:21
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
#ifndef LOG_LIG_H_
#define LOG_LIG_H_

#include <stdio.h>
#include <unistd.h>
#include <syslog.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#define LOGFILE   "FastDBCache"


struct err_open_file {
    int fd;
    char *name;
};				/* ----------  end of struct err_open_file  ---------- */

typedef struct err_open_file Err_open_file;

struct err_str_t {
    size_t      len;
    char     *data;
};				/* ----------  end of struct err_str_t  ---------- */

typedef struct err_str_t Err_str_t;

#define err_string(str)     { sizeof(str) - 1, (char *) str }
#define err_null_string     { 0, NULL }

Err_open_file *pools_elog;

/*
typedef struct log_context{
    int log_level;
    int log_fd;
    char *log_buff;
    pthread_mutex_t log_thread_log;

} LogContext;
*/
void d_log(const char *logs);
void eprintf ( const char *format, ... );

#endif /* LOG_LIG_H_ */
/* vim: set ts=4 sw=4: */

