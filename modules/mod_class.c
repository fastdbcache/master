/*
 * Author: vyouzhi <vyouzhi@163.com>
 * http://www.xda.cn
 *
 * File: mod_class.c
 * Create Date: 2012-09-04 11:31:54
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mod_class.h"
#include "../json_lib.h"
#include "../mysql_lib.h"
#include "../dict_lib.h"
#include "../command.h"

int mod_class(const char *mod_var, char **dst){
	int len;
	char gids[]="gid", *gid, pages[]="page", *page;
	char sql[200], *mod_res;
	char sql_format[] = "SELECT id,name FROM `category` where pid=%s limit %s,15";
	DICT_LINK *dict_head;
	
	json_t *json_root;

	len = 0;
	if(NULL == mod_var) return -1;

	json_root = Json_Root(mod_var);

	if(json_root == NULL) return -1;

	gid = Json_Find_Tree(gids, json_root);
	page = Json_Find_Tree(pages, json_root);

	snprintf(sql, 199, sql_format, gid, page);
	
	dict_head = mysql_fetch_all(sql, 0);
	if(NULL == dict_head) return -1;
	mod_res = Json_Set(dict_head);	
	if(NULL == mod_res) return -1;

	len = strlen(mod_res);
	if(len < 1) return -1;

	*dst = (char *)calloc(sizeof(char), (len+1));
	memcpy(*dst, mod_res, len);

	Json_Free_Root(json_root);
	dict_free(dict_head);
	free(mod_res);
	free(gid);
	free(page);

	return len;	
}

/* vim: set ts=4 sw=4: */

