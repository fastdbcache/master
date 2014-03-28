/*
 * =====================================================================================
 *
 *       Filename:  json.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/27/2014 03:55:22 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef json_H
#define json_H
#ifdef __cplusplus
 extern "C"
 {
#endif
#include "config_global.h"

#define NUMBER_LENGTH 10

enum __json_class {
    J_STRING=0;
    J_NUMBER;
    J_OBJECT;
    J_ARRAY;
    J_TRUE;
    J_FALSE;
    J_NULL;
    J_NEXT;
    J_END;
};				/* ----------  end of enum __json_class  ---------- */

typedef enum __json_class _JSONC;
#ifdef __cplusplus
}
#endif
#endif /* --- #json_H ---*/
/* vim: set ts=4 sw=4: */

