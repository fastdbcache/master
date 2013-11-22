/*
 * Author: vyouzhi <vyouzhi@163.com>
 * http://www.xda.cn
 *
 * File: SOCKET_H
 * Create Date: 2011-08-05 20:20:49
 *
 */
#ifndef SOCKET_LIB_H_
#define SOCKET_LIB_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <stdio.h>


#define MAXBUFFSIZE 65535
#define MAXCONN 10000

int SetNonBlocking(int s);
int SetUnsetBlocking(int s);
int server_socket_unix(void);
int new_socket_unix(void); 
int Set_SockOpt(int sockfd);
int Client_Init(char *host, int port);
int Socket_Init(void);
int Content_Length(char *Buf);
size_t Socket_Read(int c_fd, void *buf, ssize_t len);
size_t Socket_Send(int c_fd, const void *buf, ssize_t len);
void Socket_Close(int sock_fd);

#endif /* SOCKET_LIB_H_ */
/* vim: set ts=4 sw=4: */

