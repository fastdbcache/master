/*
 * =====================================================================================
 *
 *       Filename:  CONFIG_GLOBAL.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/20/2013 03:21:18 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef CONFIG_GLOBAL_H
#define CONFIG_GLOBAL_H
#ifdef __cplusplus
 extern "C"
 {
#endif

typedef struct _conn conn;
struct _conn{
    char *listen_ip;
    ssize_t format_len;
    MSGFORMAT *next;
};


#ifdef __cplusplus
 }
#endif
#endif /* --- #CONFIG_GLOBAL_H ---*/
 /* vim: set ts=4 sw=4: */

