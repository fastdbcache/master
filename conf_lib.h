/*
 *   Author:
 *   vyouzhi <vyouzhi@163.com>
 *   http://www.xda.cn
 *
 *   File:                    conf_lib.h
 *   Create Date:      2011-07-10 10:21:10
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
#ifndef CONF_LIB_H_     
#define CONF_LIB_H_

char *Trims(char *srcVal);
void conf_init(char *path);
char *conf_get(char *val);

#endif /* CONF_LIB_H_ */
/* vim: set ts=4 sw=4: */

