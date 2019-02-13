/*
 *@author: Jie Feng
 *@desription: this file implements the accept_mutex
 *@date: 19-2-13 上午11:45
 */
#ifndef _plusplusi_accept_mutex_H
#define _plusplusi_accept_mutex_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctime>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <csignal>
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>

#define BUFFLEN 1024
#define SERVER_PORT 1114
#define BACKLOG 5
#define PIDNUMB 2

//https://blog.csdn.net/gettogetto/article/details/77430852

pthread_mutex_t *plusplisi_accept_mutex;/* actual plusplisi_accept_mutex will be in shared memory */
void accept_mutex_init()
{
    pthread_mutexattr_t mutexattr;
    int fd = open("/dev/zero", O_RDWR, 0);
    if (fd == -1)
        printf("open err :%s\n", strerror(errno));
    plusplisi_accept_mutex = static_cast<pthread_mutex_t *>(mmap(NULL, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
    close(fd);

    pthread_mutexattr_init(&mutexattr);
    pthread_mutexattr_setpshared(&mutexattr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(plusplisi_accept_mutex, &mutexattr);
}

void accept_mutex_lock()
{
    pthread_mutex_lock(plusplisi_accept_mutex);
}

void accept_mutex_unlock()
{
    pthread_mutex_unlock(plusplisi_accept_mutex);
}

static void handle_connect(int s_s, int pid)
{
    int s_c;    /*客户端套接字文件描述符*/
    struct sockaddr_in from;    /*客户端地址*/
    socklen_t len = sizeof(from);
    /*主处理过程*/
    while (true)
    {
        /*接收客户端连接*/
        accept_mutex_lock();//add write lock
        s_c = accept(s_s, (struct sockaddr *)&from, &len);
        accept_mutex_unlock();//release write lock
        if (s_c > 0)
            printf("pid[%d] accept a client[%d]\n", pid, s_c);
        else
            printf("pid[%d] accept fail\n", pid);
        char buff[BUFFLEN];/*收发数据缓冲区*/
        memset(buff, 0, BUFFLEN);/*清零*/
        /* 发送响应给客户端 */
        sprintf(buff, "HTTP/1.0 200 OK\r\n");
        send(s_c, buff, strlen(buff), 0);
        strcpy(buff, "Server: plusplus/0.1 \r\n");
        send(s_c, buff, strlen(buff), 0);
        sprintf(buff, "Content-Type: text/html\r\n");
        send(s_c, buff, strlen(buff), 0);
        strcpy(buff, "\r\n");
        send(s_c, buff, strlen(buff), 0);
        sprintf(buff, "<html><title>plusplusi</title><body><h1>Hello World</h1></body></html>\r\n");
        send(s_c, buff, strlen(buff), 0);
        /*关闭客户端*/
        close(s_c);
    }

}

void sig_int(int num)
{
    exit(1);
}

#endif //_plusplusi_accept_mutex_H
