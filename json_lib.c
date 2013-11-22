/*
 * Author: vyouzhi <vyouzhi@163.com>
 * http://www.xda.cn
 *
 * File: json_lib.c
 * Create Date: 2011-11-18 14:50:52
 *
 */
#include "json_lib.h"
#include "dict_lib.h"


json_t *Json_Root(char *json){
    json_t *root = NULL;

    if(JSON_OK != json_parse_document (&root, json)) return NULL;
    if(NULL == root->child) return NULL;
    if(root->type == JSON_OBJECT) 
        return root;
    else
        return root->child;
}

char *Json_Find_Tree(const char *json_label, json_t *object){
    json_t *item = NULL;
    char *final_json;

    item = json_find_first_label (object, json_label);
    final_json = json_get_tree_string(item);
    json_free_value(&item);
    
    return final_json;
}

void Json_Free_Root(json_t *object){
    json_free_value(&object);
}

json_t *Json_Init_Object(void){
    json_t *jroot;

    jroot = json_new_object();
    return jroot;
}

json_t *Json_Init_Array(void){
    json_t *recinfo;
    
    recinfo = json_new_array();
    return recinfo;
}

void Json_Add_Node(char *lable, char *value, json_t **object){
    char *field, *escaped;

    escaped = json_escape(value);
    if(escaped == NULL) escaped = strdup("");
    field = string_to_json(lable);

    json_insert_pair_into_object(*object, field, json_new_string(escaped));

    free(escaped);
    free(field);

}

void Json_Add_Tree(json_t **root, json_t **node, char *node_name){
    json_insert_pair_into_object(*root, node_name, *node);
}

json_t *dict_link_to_json_t(DICT_LINK *link_head){
    DICT_LINK *dict_link_run;
	DICT_ARRAY *dict_array_run;
    json_t *jroot;
    json_t *fileinfo; 

    dict_link_run = link_head->next;
    jroot = json_new_array();

	while(dict_link_run != NULL){
		dict_array_run = dict_link_run->da;
        fileinfo = json_new_object();
		while(dict_array_run != NULL) {
            Json_Add_Node(dict_array_run->field, dict_array_run->value, &fileinfo);
			dict_array_run = dict_array_run->next;
		}
        json_insert_child(jroot, fileinfo);
		dict_link_run = dict_link_run->next;
	}

    return jroot;
}

char *Json_to_String(json_t *object){
    char *sjson, *final_json;

    json_tree_to_string(object, &sjson); 
    final_json = json_format_string(sjson);
    free(sjson);
    
    return final_json;
}

char *Json_Set(DICT_LINK *link_head){
    char *final_json;
    json_t *jroot;
   
    jroot = dict_link_to_json_t(link_head);
    final_json = Json_to_String(jroot);
    
    Json_Free_Root(jroot);

    return final_json;
}
/* vim: set ts=4 sw=4: */

