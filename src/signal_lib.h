/*
 * Author: vyouzhi <vyouzhi@163.com>
 * http://www.xda.cn
 *
 * File: SIGNAL_LIB
 * Create Date: 2011-09-26 15:57:19
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
#ifndef SIGNAL_LIB_H_
#define SIGNAL_LIB_H_

#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include "config_global.h"

void sig_handler(const int sig);
void set_signal(void);
#endif /* SIGNAL_LIB_H_ */
/* vim: set ts=4 sw=4: */

