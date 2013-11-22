/*
 * Author: vyouzhi <vyouzhi@163.com>
 * http://www.xda.cn
 *
 * File: json_lib.h
 * Create Date: 2011-11-18 14:50:56
 *
 */
#ifndef JSON_LIB_H_
#define JSON_LIB_H_

#include <malloc.h>

#include "json.h"
#include "dict_lib.h"

json_t *Json_Root(char *json);
char *Json_Find_Tree(const char *json_label, json_t *object);
void Json_Free_Root(json_t *object);
json_t *Json_Init_Object(void);
json_t *Json_Init_Array(void);
void Json_Add_Node(char *lable, char *value, json_t **object);
void Json_Add_Tree(json_t **root, json_t **node, char *node_name);
char *Json_to_String(json_t *object);
json_t *dict_link_to_json_t(DICT_LINK *link_head);
char *Json_Set(DICT_LINK *link_head);

#endif /* JSON_LIB_H_ */
/* vim: set ts=4 sw=4: */
