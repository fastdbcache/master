/*
 * Author: vyouzhi <vyouzhi@163.com>
 *
 * File: error_lib.c
 * Create Date: 2011-10-10 14:38:14
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
#include "error_lib.h"


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  error_init
 *  Description:  
 * =====================================================================================
 */

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
}		/* -----  end of function error_init  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  error_add
 *  Description:  
 * =====================================================================================
 */

void error_add(pid_t pid){

	user_errls->pid = pid;

	user_errls = user_errls->next;	
}		/* -----  end of function error_add  ----- */



/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  error_find
 *  Description:  
 * =====================================================================================
 */

ERROR_LIST *error_find(void){
	ERROR_LIST *fel;
	pid_t pid;
	
	pid = pthread_self();
	
	fel = err_list;

	for(;fel && fel->pid != pid; fel=fel->next);

	return fel;
}		/* -----  end of function error_find  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  error_store
 *  Description:  
 * =====================================================================================
 */

void error_store(int eid){
	ERROR_LIST *sel;

	sel = error_find();

    if(NULL != sel){
    	sel->reflag = eid;
    }
}		/* -----  end of function error_store  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  error_get
 *  Description:  
 * =====================================================================================
 */
int error_get(void){
	ERROR_LIST *gel;
    int el;

	gel = error_find();

    el = gel->reflag;
    gel->reflag = 0;
    
	return el;
}		/* -----  end of function error_get  ----- */

/* vim: set ts=4 sw=4: */

