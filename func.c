/*
 * Author: vyouzhi <vyouzhi@163.com>
 * http://www.xda.cn
 *
 * File: func.c
 * Create Date: 2012-07-17 15:15:58
 *
 */
#include <string.h>
#include <zlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "func.h"

void file_put_contents(const char *filename, const char *value, unsigned long val_len){
    FILE *fp;    

    fp = fopen(filename, "a+");
    fwrite(value, sizeof(char), val_len, fp);
    fclose(fp);
}

void mv(const char *dst, const char *src){
    rename(src, dst);
    unlink(src);
}

char *frand(void){
    char str[64] = "t00uAv1wB2xCy3Dz4EF5Gs6HaIb7cJdK8eLf9gMhNOiPjQkRmSnTUoVWqXYpZr";
    int i, start, end;
    char tmp[2];
    unsigned long l;
    char str_rand[9], *p;

    l = time(0);

    start = l%50 + 1;
    end = start + 9;
    str_rand[0]='\0';
    srand((unsigned int)(l%50));

    for(i=start; i<end; i++){ 
        snprintf(tmp, 2, "%c",str[(rand()%i)+1]);
        strcat(str_rand, tmp);
    }
    p = strdup(str_rand);
    return p;
}

unsigned long files_size(char *filename){
    struct stat buf;
    if(stat(filename, &buf)<0){
        return 0;
    }
    return (unsigned long)buf.st_size;
}

int files_uid(char *filename){
    struct stat buf;
    if(stat(filename, &buf)<0){
        return -1;
    }
    return buf.st_uid;
}

int files_iswrite(char *filename){
    struct stat buf;
    if(stat(filename, &buf)<0){
        return -1;
    }
    return ((buf.st_mode & S_IWGRP) != 0)?1:0;
}

int decompress_one_file(char *infilename, char *outfilename){
    gzFile infile = gzopen(infilename, "rb");
    /*FILE *outfile = fopen(outfilename, "wb");*/
    char buffer[128];
    int num_read = 0;

    if (!infile ) return -1;

    while ((num_read = gzread(infile, buffer, sizeof(buffer))) > 0) {
        gzwrite(buffer, outfilename, num_read);
    }
    gzclose(infile);
    /*fclose(outfile);*/
    return 0;
}

int mem_uncompress(const char *src,unsigned long src_len, char **dst , unsigned long *dst_len){
    int factor=1, status;
    
    do {
        *dst_len = src_len * (1 << factor++);
        *dst = (char *)calloc(sizeof(char), *dst_len + 1);
        status = uncompress((unsigned char *)*dst, dst_len, (unsigned const char *)src, src_len);
    } while (status == Z_BUF_ERROR && factor < 16);

    if(status == Z_OK){
        return 0;
    }
    
    free(*dst);
    return status;
}

/* vim: set ts=4 sw=4: */

