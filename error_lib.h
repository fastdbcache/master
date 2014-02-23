/*
 * Author: vyouzhi <vyouzhi@163.com>
 * http://www.xda.cn
 *
 * File: error_lib.h
 * Create Date: 2011-10-10 14:35:59
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

