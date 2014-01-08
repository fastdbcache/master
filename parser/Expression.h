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
void _lysave(_ly *sly, char *s, int len);
/* void _len(int l);

_ly *_get();*/

#ifdef __cplusplus
 }
#endif
#endif /* --- #Expression_H ---*/
 /* vim: set ts=4 sw=4: */

