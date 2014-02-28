/*
 * Author: vyouzhi <vyouzhi@163.com>
 *
 * File: socketbase.c
 * Create Date: 2011-08-05 20:20:44
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


#include <time.h>
#include <sys/time.h>
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "time_lib.h"


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  get_logname
 *  Description:  
 * =====================================================================================
 */

char *get_logname(void){
	time_t clocker;
    struct tm *tm;
	char buf[10], bufs[10], *p;
    
    time(&clocker);
    tm = localtime(&clocker);
    strftime(buf, sizeof(buf), "%Y%W", tm);

	sprintf(bufs, "%s", buf);
	p = (char *)calloc(sizeof(char), 10);
	strcpy(p, bufs);
	return p;
}		/* -----  end of function get_logname  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  get_tm_format
 *  Description:  format[]="%Y%m%d"
 * =====================================================================================
 */

char *get_tm_format(const char *tm_format){
    
	time_t clocker;
    struct tm *tm;
	char buf[20], bufs[20], *p;

    time(&clocker);
    tm = localtime(&clocker);
    strftime(buf, sizeof(buf), tm_format, tm);

	sprintf(bufs, "%s", buf);
	p = (char *)calloc(sizeof(char), 20);
	strcpy(p, bufs);
	return p;
}		/* -----  end of function get_tm_format  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  get_usec
 *  Description:  
 * =====================================================================================
 */

double get_usec(void){

	struct timeval tv; 
    struct timezone tz; 
    double res;
    bzero(&tv, sizeof(&tv));
    bzero(&tz, sizeof(&tz));

    gettimeofday (&tv , &tz);

    res = (double)tv.tv_sec + (double)tv.tv_usec/1000000; 
    return res;

}		/* -----  end of function get_usec  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  get_sec
 *  Description:  
 * =====================================================================================
 */

long get_sec(void){

	struct timeval tv; 
    struct timezone tz; 

    bzero(&tv, sizeof(&tv));
    bzero(&tz, sizeof(&tz));

    gettimeofday (&tv , &tz);

    return tv.tv_sec; 
}		/* -----  end of function get_sec  ----- */

/* vim: set ts=4 sw=4: */

