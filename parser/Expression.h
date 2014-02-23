/*
 * =====================================================================================
 *
 *       Filename:  Expression.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/07/2014 06:23:03 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), 
 *   Organization:  
 *
 * =====================================================================================
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
#ifndef Expression_H
#define Expression_H
#ifdef __cplusplus
 extern "C"
 {
#endif

#include "../config_global.h"
#include "Parser.h"
#include "Lexer.h"

_ly *parser_do (char *str, int len);
_ly  *_init_ly();
void _lysave(_ly *sly, char *_src);
void freely ( _ly *_fly);

#ifdef __cplusplus
 }
#endif
#endif /* --- #Expression_H ---*/
 /* vim: set ts=4 sw=4: */

