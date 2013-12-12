/*
 * Author: vyouzhi <vyouzhi@163.com>
 * http://www.xda.cn
 *
 * File: time_lib.c
 * Create Date: 2011-07-14 10:04:51
 *
 */

#include <time.h>
#include <sys/time.h>
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "time_lib.h"

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
}

/* format[]="%Y%m%d" */
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
}

double get_usec(void){

	struct timeval tv; 
    struct timezone tz; 
    double res;
    memset(&tv, 0, sizeof(&tv));
    memset(&tz, 0, sizeof(&tz));

    gettimeofday (&tv , &tz);

    res = (double)tv.tv_sec + (double)tv.tv_usec/1000000; 
    return res;
}

long get_sec(void){

	struct timeval tv; 
    struct timezone tz; 

    memset(&tv, 0, sizeof(&tv));
    memset(&tz, 0, sizeof(&tz));

    gettimeofday (&tv , &tz);

    return tv.tv_sec; 
}
/* vim: set ts=4 sw=4: */

