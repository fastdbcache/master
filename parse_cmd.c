/*
 * Author: vyouzhi <vyouzhi@163.com>
 * http://www.xda.cn
 *
 * File: parse_cmd.c
 * Create Date: 2012-09-03 17:44:46
 *
 */
#include <string.h>
#include <stdio.h>

#include "parse_cmd.h"
#include "json.h"
#include "json_lib.h"
#include "./modules/modules.h"


int parse_process(char **dst_data, char *src_data){
	char action[]="action", *module;
	char arg[]="arg", *var, *arg_object; 
	int dst_len;
	MOD_FUNC *modfc;
	json_t *json_root;
	
	module = NULL;

	json_root = Json_Root(src_data);
	if(NULL == json_root)return -1;
	
	module = Json_Find_Tree(action, json_root);
	Json_Free_Root(json_root);	

	if(NULL == module) return -1;
	MOD_INIT_FUN(modfc, module);
	free(module);

	if(NULL == modfc) return -1;
	if(NULL == modfc->ptrFunc) return -1;

	dst_len = modfc->ptrFunc(src_data, dst_data);
		
	return dst_len;
}

/* vim: set ts=4 sw=4: */

