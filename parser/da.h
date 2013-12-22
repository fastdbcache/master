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
typedef struct __ly _ly;
struct __ly {
    char *tab;
    int len;
    _ly *next;
};
_ly *ly, *tail;
_ly *parser_do (char *str, int len);
_ly  *_init_ly();
void _len(int l);
void _save(char *s, int len);
_ly *_get();


#ifdef __cplusplus
 }
#endif
#endif /* --- #da_H ---*/
 /* vim: set ts=4 sw=4: */

