/*
 * Author: vyouzhi <vyouzhi@163.com>
 * http://www.xda.cn
 *
 * File: command.h
 * Create Date: 2012-07-23 16:12:22
 *
 */
#ifndef COMMAND_H_
#define COMMAND_H_

#include <sys/types.h>

#define MCM_CSTRLEN(_str) (sizeof(_str) - 1)

/* end  */
static const char	str_end[] = "\r\nEND\r\n";
static const size_t	str_end_len = MCM_CSTRLEN(str_end);
static const char	str_space[] = " ";
static const size_t	str_space_len = MCM_CSTRLEN(str_space);

/* command */
static const char	str_feth[] = "FETH\r\n";
static const size_t	str_feth_len = MCM_CSTRLEN(str_feth);
static const char	str_back[] = "BACK\r\n";
static const size_t	str_back_len = MCM_CSTRLEN(str_back);
static const char	str_push[] = "PUSH\r\n";
static const size_t	str_push_len = MCM_CSTRLEN(str_push);
static const char	str_stored[] = "STORED\r\n";
static const size_t	str_stored_len = MCM_CSTRLEN(str_stored);
static const char str_error[] = "ERROR %d\r\n";

static const char	str_errcmd[] = "command error!\r\n";
static const size_t	str_errcmd_len = MCM_CSTRLEN(str_errcmd);
static const char	str_errdata[] = "data error!\r\n";
static const size_t	str_errdata_len = MCM_CSTRLEN(str_errdata);
static const char	str_errbytes[] = "name command the <bytes>  error!\r\n";
static const size_t	str_errbytes_len = MCM_CSTRLEN(str_errbytes);
static const char	str_errfn[] = "name command the <file name>  error!\r\n";
static const size_t	str_errfn_len = MCM_CSTRLEN(str_errfn);
static const char	str_errsuffix[] = "the file is not apk  error!\r\n";
static const size_t	str_errsuffix_len = MCM_CSTRLEN(str_errsuffix);


/* Set various error codes */
#define SNK_ERR_NONE			 0
#define SNK_ERR_DATA		 	 -1
#define SNK_ERR_RECV_NONE     -2
#define SNK_ERR_QUIT		 -3
#define SNK_ERR_COMMAND		 -4
#define SNK_ERR_SUFFIX     -5
#define SNK_ERR_BYTES    -6
#define SNK_ERR_FN      -7 /* file name error */
#define SNK_ERR_RECV_BUF_FULL -8   /* much data recv , so i must it as xxx_tmp  */
#define SNK_ERR_DATE_NOTENOUGH  -9   /*  the big data is not enough */
#define SNK_ERR_CLIENT_QUIT -10  /* the client is not normal quit*/

/* only accept apk ... */
static const char *SNKsuffix[] = {".apk", ".APK"};
typedef unsigned int uint32;	

#define __DEBUG

#ifdef __DEBUG
#define DEBUG(info)    printf(info)
#else
#define DEBUG(info)
#endif

#endif /* COMMAND_H_ */

/* vim: set ts=4 sw=4: */

