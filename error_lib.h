/*
 * Author: vyouzhi <vyouzhi@163.com>
 * http://www.xda.cn
 *
 * File: error_lib.h
 * Create Date: 2011-10-10 14:35:59
 *
 */
#ifndef ERROR_LIB_H_
#define ERROR_LIB_H_

#include <pthread.h>
#include <malloc.h>

#define ERROR_QUERY 1
#define ERROR_SQL 2

typedef struct error_list ERROR_LIST;
struct error_list {
    int	eflag;
	int reflag;
	pid_t pid;
    ERROR_LIST *next;     
};

ERROR_LIST *err_list;
ERROR_LIST *user_errls;

void error_init(int error_num);
void error_add(pid_t pid);
ERROR_LIST *error_find(void);
void error_store(int eid);
int error_get(void);
#endif /* ERROR_LIB_H_ */
/* vim: set ts=4 sw=4: */

