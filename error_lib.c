/*
 * Author: vyouzhi <vyouzhi@163.com>
 * http://www.xda.cn
 *
 * File: error_lib.c
 * Create Date: 2011-10-10 14:38:14
 *
 */

#include "error_lib.h"

void error_init(int error_num){
	int i;
	ERROR_LIST *el;

	err_list = NULL;

	for(i=0; i<error_num; i++){
		el = (ERROR_LIST *)calloc(1, sizeof(ERROR_LIST));
		el->next = err_list;
		err_list = el;
        
	} 
	user_errls = err_list;
}

void error_add(pid_t pid){

	user_errls->pid = pid;

	user_errls = user_errls->next;	

}

ERROR_LIST *error_find(void){
	ERROR_LIST *fel;
	pid_t pid;
	
	pid = pthread_self();
	
	fel = err_list;

	for(;fel && fel->pid != pid; fel=fel->next);

	return fel;
}

void error_store(int eid){
	ERROR_LIST *sel;

	sel = error_find();

    if(NULL != sel){
    	sel->reflag = eid;
    }

}

int error_get(void){
	ERROR_LIST *gel;
    int el;

	gel = error_find();

    el = gel->reflag;
    gel->reflag = 0;
    
  
	return el;
}
/* vim: set ts=4 sw=4: */

