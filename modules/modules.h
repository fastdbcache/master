/*
 * Author: vyouzhi <vyouzhi@163.com>
 * http://www.xda.cn
 *
 * File: modules.h
 * Create Date: 2012-09-04 10:17:35
 *
 */
#ifndef MODULES_H_
#define MODULES_H_

#define MOD_VALID_FUNC_SIG	const char *ACT, char **DST

typedef int	 (*modPtrFunc)(MOD_VALID_FUNC_SIG);

#define MOD_INIT_FUN(mod_func, mod_action) do{\
	for((mod_func)=(mod_func_head)->next; (mod_func)!=NULL; (mod_func)=(mod_func)->next){    \
		if(strcmp((mod_func)->mod_name, mod_action)==0)break;   \
	}                   \
}while (0)

#define MOD_REGISTER(mod_names, mod_func) do{ \
	MOD_FUNC *emf, *head;   \
	head = mod_func_head;  \
	emf = (MOD_FUNC*)calloc(sizeof(MOD_FUNC), 1);  \
	emf->mod_name = mod_names;  \
	emf->ptrFunc = mod_func;  \
	emf->next = NULL;  \
	MOD_ADD(head, emf); \
}while (0)

#define MOD_FOREACH(var)   \
	for(;(var)->next!=NULL; (var)=(var)->next)

#define MOD_ADD(head, eml) do{  \
	MOD_FOREACH(head); \
	head->next = eml; \
}while(0)

typedef struct mod_func_conn{
	char *mod_name;
	modPtrFunc ptrFunc;
	struct mod_func_conn *next;
} MOD_FUNC;

MOD_FUNC *mod_func_head;

void modules_register(void);

#endif /* MODULES_H_ */
/* vim: set ts=4 sw=4: */

