/*
 *   Author:
 *   vyouzhi <vyouzhi@163.com>
 *   http://www.xda.cn
 *
 *   File:                    conf_lib.c
 *   Create Date:      2011-07-10 10:21:07
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "conf_lib.h"

char *Trims(char *srcVal){
	char *cp1 = srcVal, *cp2 = srcVal;

	if(srcVal){
		while(*cp1 == ' ')
			cp1++;

		while(*cp1)
			*cp2++ = *cp1++;

		if(cp2 != srcVal){
			while(*--cp2 == ' ');
			*(cp2+1) = 0;
		}else
		   *srcVal = 0;
	}
	return srcVal;
}

void conf_init(char *path){
	FILE *fp;
    char *p, *right_val, *left_val;
	char buf[100], bufs[100];
	fp = fopen(path, "r");
	assert((fp!=NULL));
	while(!feof(fp)){
		fgets(buf,2, fp);
		if(!strcmp(buf, "#")){
			fgets(buf, 100, fp);
		}else if(!strcmp(buf, "[")){
			fgets(buf, 100, fp);
		}else{
			fgets(bufs, 100, fp);
	
			if(bufs[strlen(bufs)-1] == '\n')
				bufs[strlen(bufs)-1] = ' ';

			p = strcat(buf, bufs);
			left_val = strtok(p, "=");
			right_val = strtok(NULL, "=");

			left_val = Trims(left_val);
			right_val = Trims(right_val);

			if(left_val == NULL)continue;

			if(right_val == NULL) setenv(left_val,"",1);			
			else setenv(left_val, right_val, 1);

		}
	}
	fclose(fp);
}

char *conf_get(char *val){
	char *p;
	if((p = getenv(val)) == NULL){
		printf("can't get val conf: %s\n", val);
	}
	return p;
}


/* vim: set ts=4 sw=4: */

