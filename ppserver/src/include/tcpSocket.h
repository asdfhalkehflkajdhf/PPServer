#ifndef __TCP_SOCKET_H__
#define __TCP_SOCKET_H__

#ifdef __cplusplus 
extern "C" {
#endif
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/select.h>
#include <sys/time.h>

//int CloseSocket(int sockfd);


#define  select_fd_init(fdset)             FD_ZERO(fdset)
#define  select_fd_del(fdset, fd)        FD_CLR(fd, fdset)
#define  select_fd_add(fdset, fd)       FD_SET(fd, fdset)

int select_fd_check_r(fd_set *fdset_r, int fd, int sec, int usec);
int select_fd_check_w(fd_set *fdset_w, int fd, int sec, int usec);
int select_fd_check_rw(fd_set *fdset_r, fd_set *fdset_w,int fd, int sec, int usec);

int select_fd_check2(fd_set *fdset, int fd);

int select_fd_timeout_r(int fd, int sec, int usec);
int select_fd_timeout_w(int fd, int sec, int usec);
int select_fd_timeout_rw(int fd, int sec, int usec);

//int CloseSocket(int sockfd);

int SWriten(int sckid, char *buf, int len, int sec, int usec);
int SReadn(int sckid, char *buf, int len, int sec, int usec);
int CheckConnect0(int iSocket);

int SConnectRemote(char *ip, int port, int sec, int usec);
int SListenRemote(struct sockaddr_in *server_addr, int port, int ShortConn);
int SAcceptRemote(struct sockaddr_in *remote_addr, int sockfd);
void SsetOpetinNoBlock(int fd);
void SClearRecvBuff(int Socket);

#ifdef __cplusplus
}
#endif 

#endif

