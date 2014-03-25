/*
 * =====================================================================================
 *
 *       Filename:  dep_misc.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年01月05日 02时02分53秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), vyouzhi@gmail.com
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
#include "dep_struct.h"

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  deposit_init
 *  Description:  
 * =====================================================================================
 */
DEPO *deposit_init (  ){
    DEPO *_depo;
    _depo = calloc(1, sizeof(DEPO));
    _depo->sm = NULL;
    _depo->ss = 0;
    _depo->sp = 0;
    _depo->se = 0;
    return _depo;
}		/* -----  end of function deposit_init  ----- */

 /* vim: set ts=4 sw=4: */


