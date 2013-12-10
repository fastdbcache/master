/*
 * =====================================================================================
 *
 *       Filename:  da.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/10/2013 10:21:44 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef da_H
#define da_H
#ifdef __cplusplus
 extern "C"
 {
#endif
void parser_do (char *str, int len);
void _init_con();
void _len(int l);
void _save(char *s, int len);
void _get();
typedef struct conns conn;
struct conns {
    char *tab;
    int len;
};
conn *_conn;

#ifdef __cplusplus
 }
#endif
#endif /* --- #da_H ---*/
 /* vim: set ts=4 sw=4: */

