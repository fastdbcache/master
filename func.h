/*
 * Author: vyouzhi <vyouzhi@163.com>
 * http://www.xda.cn
 *
 * File: func.h
 * Create Date: 2012-07-17 15:16:01
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
#ifndef FUNC_H_
#define FUNC_H_

#include <stdlib.h>

#define SPACE 0x20
#define ENTER '\n'
#define DOT '\''

#define HEAD_SHIFT(head, dst, step, except) do {	\
    char *hsp = (head);  \
    for(;step>0&&*hsp!='\0'&&hsp!=NULL;hsp++)if(*hsp==except){dst=hsp;step--;} \
} while (0)

#define OFFSET_SHIFT(head, offset, except) do {				\
    char *cp = head; \
    do{               \
        (cp)++;    \
        (offset)++; \
        if(cp==NULL || *cp=='\0'){ \
            offset=0;   \
            break; \
        }   \
    } while(*cp!=except);\
} while (0)


void mv(const char *dst, const char *src);
char *frand(void);
void file_put_contents(const char *filename, const char *value, unsigned long val_len);
unsigned long files_size(char *filename);
int files_uid(char *filename);
int files_mode(char *filename);

int decompress_one_file(char *infilename, char *outfilename);
int mem_uncompress(const char *src,unsigned long src_len, char **dst , unsigned long *dst_len);
int emptys(const char *pointer);
#endif /* FUNC_H_ */
/* vim: set ts=4 sw=4: */

