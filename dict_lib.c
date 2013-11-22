/*
 * Author: vyouzhi <vyouzhi@163.com>
 * http://www.xda.cn
 *
 * File: dict_lib.c
 * Create Date: 2011-11-07 11:42:29
 *
 */

#include "dict_lib.h"

DICT_LINK *dict_init(void){
	DICT_LINK *dl;
	dl = (DICT_LINK *)calloc(1, sizeof(DICT_LINK));
	dl->da = NULL;
	return dl;
}

DICT_ARRAY *dict_add(FieldName const *fields, char **value, unsigned int num_field){
	DICT_ARRAY *array_run, *array_head;
	FieldName const *fn_run;
	int m = num_field-1;

	fn_run = fields;
	array_head = NULL;

	while(fn_run != NULL && fn_run->fn != NULL){
		array_run = (DICT_ARRAY *)calloc(1, sizeof(DICT_ARRAY));
   		array_run->value = value[m]!=NULL ? strdup(value[m]) : value[m];
		array_run->field = fn_run->fn ? strdup(fn_run->fn) : fn_run->fn;
        array_run->next = array_head; 
		array_head = array_run;
		fn_run = fn_run->next;
		m--;
	}
	
	return array_head;
}

int dict_length(DICT_LINK *head){
    DICT_LINK *l;
    int i=0;
    l = head;
    for(;l;l=l->next)i++;

    return i;
}

DICT_LINK *dict_find_link_base(DICT_LINK *head, DICT_ARRAY *da_head, int except){
    DICT_LINK *l, *h_pre;
    DICT_ARRAY *da;
    l = h_pre = head;

    for(;l;l=l->next){
        da = l->da;
        for(;da;da=da->next)
            if(strcmp(da->field, da_head->field)==0 && strcmp(da->value, da_head->value)==0){
                if(except == 1){
                    h_pre->next = l->next;
                    l->next = NULL;
                }
                return l;
            }
        h_pre = l;
    }

    return l;
}

DICT_ARRAY *dict_find_array(DICT_ARRAY *head, char *find_field){
    DICT_ARRAY *d;
    d = head;
    
    for(;d;d=d->next){
        if(strcmp(d->field, find_field) == 0)return d;
    }

    return d;
}


DICT_LINK *dict_except_link(DICT_LINK *head, DICT_ARRAY *except){
    return dict_find_link_base(head, except, 1);
}

DICT_LINK *dict_find_link(DICT_LINK *head, DICT_ARRAY *da_head){
    return dict_find_link_base(head, da_head, 0);
}


FieldName *field_init(void){
	FieldName *fn_head;
	fn_head = (FieldName *)calloc(1, sizeof(FieldName));
	fn_head->fn = NULL;
	return fn_head;
}

void field_add(FieldName **head, char *fn, int type){
	FieldName *nfn;
	nfn = field_init();
	
	if(fn != NULL){
		if(type == 0)
			nfn->fn = strdup(fn);
		else
			nfn->fn = strdup("_0");
	}
	nfn->next = *head;
	*head = nfn;
}

void dict_free(DICT_LINK *head){
	DICT_LINK *link_del, *link_run;
	DICT_ARRAY *array_del, *array_run;

	link_run = head;
	while(link_run != NULL){
		link_del = link_run;
		array_run = link_del->da;
		link_run = link_run->next;

		while(array_run != NULL){
			array_del = array_run;
			array_run = array_run->next;
			free(array_del->field);
			free(array_del->value);
			free(array_del);
		}
		free(link_del);
	}
}

void field_free(FieldName *head){
	FieldName *fn_del, *fn_run;
	
	fn_run = head;

	while(fn_run != NULL){
		fn_del = fn_run;
		fn_run = fn_run->next;
		free(fn_del->fn);
		free(fn_del);
	}
}

/* vim: set ts=4 sw=4: */

