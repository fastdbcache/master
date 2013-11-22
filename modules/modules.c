/*
 * Author: vyouzhi <vyouzhi@163.com>
 * http://www.xda.cn
 *
 * File: modules.c
 * Create Date: 2012-09-04 11:30:41
 *
 */
#include <stdlib.h>
#include <string.h>

#include "modules.h"

/* include module here */
#include "mod_class.h"
#include "mod_test.h"

void modules_register(void){

	mod_func_head = (MOD_FUNC *)calloc(sizeof(MOD_FUNC), 1);	

	/* mod_class modules name is mod_class, import function is mod_class  */
	MOD_REGISTER(mod_class_name, mod_class);

	MOD_REGISTER(mod_test_name, mod_test);

}

/* vim: set ts=4 sw=4: */

