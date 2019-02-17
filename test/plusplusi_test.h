/*
 *@author: Jie Feng
 *@desription: todo
 *@date: 19-2-16 上午10:56
 */
#ifndef _plusplusi_test_H
#define _plusplusi_test_H

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <pthread.h>
#include <cerrno>
#include <string>

#define MAXLINE 1024
#define LISTENQ 20
#define SERV_PORT 1114

//用于读写两个的两个方面传递参数

struct user_data
{
    int fd;
    unsigned int n_size;
    char message[MAXLINE];
};

//声明epoll_event结构体的变量,ev用于注册事件,数组用于回传要处理的事件

struct epoll_event ev, events[20];

int epfd;

//pthread_mutex_t plusplisi_accept_mutex;

//pthread_cond_t cond1;

//struct task *readhead = NULL, *readtail = NULL, *writehead = NULL;

int i, listenfd, connfd, sockfd, nfds;

unsigned int n;

struct user_data *data = nullptr;

struct user_data *rdata = nullptr;//用于读写两个的两个方面传递参数

socklen_t client_len;

struct sockaddr_in client_addr;

struct sockaddr_in server_addr;

void setnonblocking(int sock)
{
    int opts;
    opts = fcntl(sock, F_GETFL);
    if (opts < 0)
    {
        perror("fcntl(sock,GETFL)");
        exit(1);
    }
    opts = opts | O_NONBLOCK;
    if (fcntl(sock, F_SETFL, opts) < 0)
    {
        perror("fcntl(sock,SETFL,opts)");
        exit(1);
    }
}

void init()
{
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    //把socket设置为非阻塞方式
    setnonblocking(listenfd);
    int reuse_socket = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse_socket, sizeof(int)) == -1)
    {
        printf("setsockopt reuse-addr error!");
    }
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    char local_addr[] = "0.0.0.0";
    inet_aton(local_addr, &(server_addr.sin_addr));//htons(SERV_PORT);

    server_addr.sin_port = htons(SERV_PORT);
    if (bind(listenfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
        perror("bind");
    if (listen(listenfd, LISTENQ) < 0)
        perror("listen");
}

void work_cycle(int j)
{
    printf("entering work_cycle\n");

    //生成用于处理accept的epoll专用的文件描述符
    //epfd = epoll_create(256);
    //设置与要处理的事件相关的文件描述符
    //ev.data.fd = listenfd;
    //设置要处理的事件类型
    ev.events = EPOLLIN | EPOLLET;

    //注册epoll事件
    epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);
    for (;;)
    {
        //等待epoll事件的发生
        nfds = epoll_wait(epfd, events, 20, 100000);
        //处理所发生的所有事件

        for (i = 0; i < nfds; ++i)
        {
            if (events[i].data.fd == listenfd)
            {
                connfd = accept(listenfd, (struct sockaddr *) &client_addr, &client_len);
                if (connfd < 0)
                {
                    printf("process %d:connfd<0 accept failure\n", j);
                    continue;
                }
                setnonblocking(connfd);
                char *str = inet_ntoa(client_addr.sin_addr);
                printf("process %d:connect_from >>%s listenfd=%d connfd=%d\n", j, str, listenfd, connfd);
                //设置用于读操作的文件描述符
                ev.data.fd = connfd;
                //设置用于注测的读操作事件
                ev.events = EPOLLIN | EPOLLET;
                //注册ev
                epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev);

            } else
            {
                if (events[i].events & EPOLLIN)
                {
                    printf("process %d:reading! connfd=%d\n", j, events[i].data.fd);
                    if ((sockfd = events[i].data.fd) < 0) continue;
                    data = (struct user_data *) malloc(sizeof(struct user_data));
                    if (data == nullptr)
                    {
                        printf("process %d:user_data malloc error", j);
                        exit(1);
                    }
                    data->fd = sockfd;
                    int index = 0, len = 1024, bytes_read = 0;
                    while (true)
                    {
                        bytes_read = read(sockfd, data->message + index, len - index);
                        if (bytes_read == -1)
                        {
                            if (errno == EAGAIN || errno == EWOULDBLOCK)
                                break;
                            printf("PID=%d 读错误退出\n", getpid());
                            break;
                        } else if (bytes_read == 0)
                        {
                            printf("process %d:Client close connect!\n", j);
                            close(sockfd);
                            break;
                        }
                    }
                    printf("server received\n%s\n", data->message);
                    memset(data->message, 0, sizeof(data->message));
                    std::string str =
                            "HTTP/1.1 200 OK\r\nContent-Length: " + std::to_string(strlen("this is from server")) +
                            "\r\nContent-Type: text/html\r\n\r\nthis is from server";
                    strcpy(data->message, str.c_str());
                    data->n_size = n;
                    //设置需要传递出去的数据
                    ev.data.ptr = data;
                    //设置用于注测的写操作事件
                    ev.events = EPOLLOUT | EPOLLET;
                    //修改sockfd上要处理的事件为EPOLLOUT
                    epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);

                } else if (events[i].events & EPOLLOUT)
                {
                    rdata = (struct user_data *) events[i].data.ptr;
                    sockfd = rdata->fd;
                    printf("process %d:write\n%s connfd: %d\n", j, data->message, sockfd);
                    write(sockfd, rdata->message, rdata->n_size);
                    //delete rdata;
                    free(rdata);
                    //设置用于读操作的文件描述符
                    ev.data.fd = sockfd;
                    //设置用于注测的读操作事件
                    ev.events = EPOLLIN | EPOLLET;
                    //修改sockfd上要处理的事件为EPOLIN
                    epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);
                    close(sockfd);
                }
            }
        }
    }
}

int epoll_run()
{
    int i;
    int pid;
    init();

    //生成用于处理accept的epoll专用的文件描述符
    epfd = epoll_create(256);

    //设置与要处理的事件相关的文件描述符
    ev.data.fd = listenfd;
    /*for (i = 0; i < 3; i++)
    {
        pid = fork();
        switch (pid)
        {
            case -1:
                printf("fork sub process failed!\n");
                break;
            case 0:
                work_cycle(i);
                break;
            default:
                break;
        }
    }*/

    work_cycle(0);
}

#endif //_plusplusi_test_H
