/*
 * Author: vyouzhi <vyouzhi@163.com>
 * http://www.xda.cn
 *
 * File: dict_lib.h
 * Create Date: 2011-11-07 11:42:33
 *
 */
#ifndef DICT_LIB_H_
#define DICT_LIB_H_

#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>

typedef struct fieldname{
    char *fn;
    struct fieldname *next;
} FieldName;

typedef struct dict_array{
	char *field;
	char *value;
    struct dict_array *next;
} DICT_ARRAY;

typedef struct dict_link{
	DICT_ARRAY *da;
	struct dict_link *next;
} DICT_LINK;

DICT_LINK *dict_init(void);
DICT_ARRAY *dict_add(FieldName const *fields, char **value, unsigned int num_field);
int dict_length(DICT_LINK *head);
DICT_LINK *dict_find_link(DICT_LINK *head, DICT_ARRAY *da_head);
DICT_ARRAY *dict_find_array(DICT_ARRAY *head, char *find_field);
FieldName *field_init(void);
void field_add(FieldName **head, char *fn, int type);
void dict_free(DICT_LINK *head);
void field_free(FieldName *head);
#endif /* DICT_LIB_H_ */
/* vim: set ts=4 sw=4: */

