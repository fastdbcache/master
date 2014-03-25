/*
 * Author: vyouzhi <vyouzhi@163.com>
 * http://www.xda.cn
 *
 * File: modules.c
 * Create Date: 2012-09-04 11:30:41
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

