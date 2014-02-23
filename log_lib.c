/*
 * Author: vyouzhi <vyouzhi@163.com>
 * http://www.xda.cn
 *
 * File: log_lib.c
 * Create Date: 2011-10-10 09:37:15
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
#include "log_lib.h"

void d_log(const char *logs){
	openlog(LOGFILE, LOG_PID|LOG_CONS, LOG_LOCAL0);
	syslog(LOG_ALERT,"info: %s\n", logs);
	closelog();
}

/*
static void doLog(LogContext *pContext, const char *caption, \
		const char *text, const int text_len, const bool bNeedSync)
{
	time_t t;
	struct tm tm;
	int buff_len;
	int result;

	t = time(NULL);
	localtime_r(&t, &tm);
	if ((result=pthread_mutex_lock(&pContext->log_thread_lock)) != 0)
	{
		fprintf(stderr, "file: "__FILE__", line: %d, " \
			"call pthread_mutex_lock fail, " \
			"errno: %d, error info: %s", \
			__LINE__, result, STRERROR(result));
	}

	if (text_len + 64 > LOG_BUFF_SIZE)
	{
		fprintf(stderr, "file: "__FILE__", line: %d, " \
			"log buff size: %d < log text length: %d ", \
			__LINE__, LOG_BUFF_SIZE, text_len + 64);
		pthread_mutex_unlock(&(pContext->log_thread_lock));
		return;
	}

	if ((pContext->pcurrent_buff - pContext->log_buff) + text_len + 64 \
			> LOG_BUFF_SIZE)
	{
		log_fsync(pContext, false);
	}

	buff_len = sprintf(pContext->pcurrent_buff, \
			"[%04d-%02d-%02d %02d:%02d:%02d] %s - ", \
			tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday, \
			tm.tm_hour, tm.tm_min, tm.tm_sec, caption);
	pContext->pcurrent_buff += buff_len;
	memcpy(pContext->pcurrent_buff, text, text_len);
	pContext->pcurrent_buff += text_len;
	*pContext->pcurrent_buff++ = '\n';

	if (!pContext->log_to_cache || bNeedSync)
	{
		log_fsync(pContext, false);
	}

	if ((result=pthread_mutex_unlock(&(pContext->log_thread_lock))) != 0)
	{
		fprintf(stderr, "file: "__FILE__", line: %d, " \
			"call pthread_mutex_unlock fail, " \
			"errno: %d, error info: %s", \
			__LINE__, result, STRERROR(result));
	}
}
*/

/* vim: set ts=4 sw=4: */

