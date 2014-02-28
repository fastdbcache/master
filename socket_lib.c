/*
 * Author: vyouzhi <vyouzhi@163.com>
 *
 * File: socketbase.c
 * Create Date: 2011-08-05 20:20:44
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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/un.h>
#include <sys/time.h>
#include <sys/select.h>


#include "func.h"
#include "socket_lib.h"
#include "conf_lib.h"
#include "log_lib.h"
#include "config_global.h"


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  SetNonBlocking
 *  Description:  
 * =====================================================================================
 */
int SetNonBlocking(int s){
    int opts;
    opts=fcntl(s, F_GETFL);
    if(opts == -1)
    {
        d_log("fcntl(sock,GETFL)");
        exit(-1);
    }
    opts |= O_NONBLOCK;
    if(fcntl(s, F_SETFL, opts) == -1)
    {
        d_log("fcntl(sock,SETFL,opts)");
        exit(1);
    }
	return 0;
}		/* -----  end of function SetNonBlocking  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  SetUnsetBlocking
 *  Description:  
 * =====================================================================================
 */

int SetUnsetBlocking(int s){
    int opts;
    opts=fcntl(s, F_GETFL, 0);
    if(opts == -1)
    {
        d_log("fcntl(sock,GETFL)");
        exit(-1);
    }
    if(fcntl(s, F_SETFL, opts & !O_NONBLOCK) == -1)
    {
        d_log("fcntl(sock,SETFL,opts)");
        exit(1);
    }
	return 0;
}		/* -----  end of function SetUnsetBlocking  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  new_socket_unix
 *  Description:  
 * =====================================================================================
 */
int new_socket_unix(void) {
    int sfd;

    if ((sfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        d_log("socket()");
        return -1;
    }
	SetNonBlocking(sfd);

    return sfd;
}		/* -----  end of function new_socket_unix  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  server_socket_unix
 *  Description:  
 * =====================================================================================
 */

int server_socket_unix(void) {
    int sfd;
    struct linger ling = {0, 0};
    struct sockaddr_un addr;
    struct stat tstat;
    int flags =1;
    int old_umask, access_mask = 0777;
	char *path;
	
	path = conf_get("path");

    if (!path) {
        return -1;
    }

    if ((sfd = new_socket_unix()) == -1) {
        return -1;
    }

    /*
     * Clean up a previous socket file if we left it around
     */
    if (lstat(path, &tstat) == 0) {
        if (S_ISSOCK(tstat.st_mode))
            unlink(path);
    }

    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (void *)&flags, sizeof(flags));
    setsockopt(sfd, SOL_SOCKET, SO_KEEPALIVE, (void *)&flags, sizeof(flags));
    setsockopt(sfd, SOL_SOCKET, SO_LINGER, (void *)&ling, sizeof(ling));

    /*
     * the memset call clears nonstandard fields in some impementations
     * that otherwise mess things up.
     */
    memset(&addr, 0, sizeof(addr));

    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);
    /*assert(strcmp(addr.sun_path, path) == 0);*/
    old_umask = umask( ~(access_mask&0777));
    if (bind(sfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        close(sfd);
        umask(old_umask);
        return 1;
    }
    umask(old_umask);

    if (listen(sfd, MAXCONN) == -1) {
        close(sfd);
        return 1;
    }

    return sfd;
}		/* -----  end of function server_socket_unix  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  Set_SockOpt
 *  Description:  
 * =====================================================================================
 */

int Set_SockOpt(int sockfd){
	socklen_t sendbuflen = (256*1024*1024);
    socklen_t len = sizeof(sendbuflen);
	struct linger opt;
	int nNetTimeout = 1;
    int enable = 1;
    int keepcnt = 5;
    int keepidle = 30;
    int keepintvl = 120;

	if (-1 == setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void*)&sendbuflen, len)) {
		d_log("Couldn't setsockopt(SO_REUSEADDR)\n");
		return (-1);
	}
	
	if (-1 == setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (void*)&sendbuflen, len )) {
		d_log("Couldn't setsockopt(SO_SNDBUF)\n");
		return (-1);
	}

	if (-1 == setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (void*)&sendbuflen, len )) {
		d_log("Couldn't setsockopt(SO_RCVBUF)\n");
		return (-1);
	}
	
	setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&nNetTimeout, sizeof(int));
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout, sizeof(int));


    /*
    setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT, &keepcnt, sizeof(int));
    setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE, &keepidle, sizeof(int));
    setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL, &keepintvl, sizeof(int));

    setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (char *)&nNetTimeout, sizeof(int));
        setsockopt(sockfd, SOL_SOCKET, SO_NOSIGPIPE, (char *)&nNetTimeout, sizeof(int));*/

	opt.l_onoff = 1;
	opt.l_linger = 60;

	if (-1 == setsockopt(sockfd, SOL_SOCKET, SO_LINGER, &opt,
		sizeof(struct linger))){
		d_log("Coundn't setsockopt(SO_LINGER)\n");
		return (-1);
	}

    if (setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY,
                  (void*)&enable, sizeof(enable)) == -1) {
        printf("noblock SockOpt\n");
		return (-1);
    }

	return 0;
}		/* -----  end of function Set_SockOpt  ----- */



/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  Socket_Init
 *  Description:  
 * =====================================================================================
 */

int Socket_Init(void){

	int sockfd, port;
	struct sockaddr_in   server_addr;
	char *host;
	
	host = conf_get("server_ip");
	port = atoi(conf_get("server_port"));

	bzero(&server_addr, sizeof(server_addr));
	inet_pton(AF_INET, host, &(server_addr.sin_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (-1 == sockfd) {return(-1);}
	
	if (-1 == Set_SockOpt(sockfd)) {
		close(sockfd);
		return (-1);
	}

	if (-1 == bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr))) {
		close(sockfd);
		return(-1);
	}
    if (-1 == listen(sockfd, MAXCONN)) {
		close(sockfd);
		return(-1);
	}
	
	return (sockfd);
}		/* -----  end of function Socket_Init  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  Client_Init
 *  Description:  
 * =====================================================================================
 */

int Client_Init(char *host, int port){
	int flag = 1, client_fd;
	struct sockaddr_in to_server_addr;

    if((client_fd = socket(AF_INET , SOCK_STREAM , 0)) == - 1) {
        /*d_log("socket error") ;*/
        return (-1);
    }

    /* if(-1 == SetNonBlocking(client_fd)){
        printf("noblock error\n");
        close(client_fd);
        return (-1);
    }*/
    
    memset((char *)&to_server_addr, 0, sizeof(to_server_addr)) ;
    to_server_addr.sin_family = AF_INET;
    to_server_addr.sin_port = htons(port);
    to_server_addr.sin_addr.s_addr = inet_addr(host) ;

	if(connect(client_fd, (struct sockaddr *)&to_server_addr, sizeof(struct sockaddr)) == - 1) {
        printf("can't connect backend server\n");
        return (-1);
    }
   /*  if (setsockopt(client_fd, IPPROTO_TCP, TCP_NODELAY,
                  &flag, sizeof(flag)) == -1) {
        printf("noblock SockOpt\n");
		return (-1);
    }*/
    //SetUnsetBlocking(client_fd); 
	return client_fd;
}		/* -----  end of function Client_Init  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  Socket_Read
 *  Description:  
 * =====================================================================================
 */
size_t Socket_Read(int c_fd, void *buf, ssize_t len){
    size_t nread;
    ssize_t offset=0, stop=0;
    
    do{
        nread = read(c_fd, buf+offset, (len-offset));
        if(nread < 0){
            if ( errno == EINTR || errno == EAGAIN){
                continue;
            }else return -1;
        }else if(nread == 0){
            return -1;
        }else{
            offset += nread;
            
            if(offset == len) stop=1;
        }

    }while(!stop);

    return nread;
}		/* -----  end of function Socket_Read  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  Socket_Send
 *  Description:  
 * =====================================================================================
 */

size_t Socket_Send(int c_fd, const void *buf, ssize_t len){
	ssize_t stop=0, offset=0;
    size_t nsend;
	const void *sBuf = buf;

	
    if(c_fd < 1) return -1;
    if(len < 1) return -1;
    if(sBuf == NULL) return -1;

	do{
		nsend = write(c_fd, sBuf+offset, (len-offset));
		if(nsend < 0)
		{
			if(errno == EAGAIN || errno == EINTR)
			{

                DEBUG("Oh dear, something went wrong with read()! %s", strerror(errno));
				continue;
			}
		}else if(nsend == 0)return -1;	
        
        offset += nsend;
        if(offset == len) stop = 1;
	}while(!stop);

	return offset;
}		/* -----  end of function Socket_Send  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  Socket_Close
 *  Description:  
 * =====================================================================================
 */

void Socket_Close(int sock_fd){
	close(sock_fd);
}		/* -----  end of function Socket_Close  ----- */


/* vim: set ts=4 sw=4: */

