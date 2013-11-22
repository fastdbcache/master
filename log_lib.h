/*
 * Author: vyouzhi <vyouzhi@163.com>
 * http://www.xda.cn
 *
 * File: LOG_LIG
 * Create Date: 2011-10-10 09:37:21
 *
 */
#ifndef LOG_LIG_H_
#define LOG_LIG_H_

#include <stdio.h>
#include <unistd.h>
#include <syslog.h>

#define LOGFILE   "suning_stat"
/*
typedef struct log_context{
    int log_level;
    int log_fd;
    char *log_buff;
    pthread_mutex_t log_thread_log;

} LogContext;
*/
void d_log(const char *logs);

#endif /* LOG_LIG_H_ */
/* vim: set ts=4 sw=4: */

