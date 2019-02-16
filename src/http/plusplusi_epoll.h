/*
 *@author: Jie Feng
 *@desription: this file is a epoll test file
 *@date: 19-2-13 下午12:06
 */
#ifndef _plusplusi_epoll_H
#define _plusplusi_epoll_H
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/epoll.h>
#include <unistd.h>

#include "plusplusi_accept_mutex.h"

#define WORKER_MAX        8
#define EVENT_LIST_MAX    128
#define EVENT_MAX         12

typedef int (*CALLBACK_FUN)(struct epoll_event *);

typedef struct EPOLL_DATA_S
{
    int Epoll_FD;
    int Event_FD;
    CALLBACK_FUN callback_fun;
} Epoll_Data_S;

static int create_epoll(unsigned int event_num)
{
    int epoll_fd;
    epoll_fd = epoll_create(event_num);
    if (-1 == epoll_fd)
    {
        printf("epoll create failed\r\n");
        return -1;
    }
    return epoll_fd;
}

static void add_event_epoll(int epoll_fd, int event_fd, CALLBACK_FUN callback_fun)
{
    int op = EPOLL_CTL_ADD;
    struct epoll_event ee;
    Epoll_Data_S *data;
    data = static_cast<Epoll_Data_S *>(malloc(sizeof(Epoll_Data_S)));
    if (NULL == data)
    {
        return;
    }

    data->Epoll_FD = epoll_fd;
    data->Event_FD = event_fd;
    data->callback_fun = callback_fun;
    ee.events = EPOLLIN | EPOLLOUT | EPOLLHUP;
    //ee.events = EPOLLIN|EPOLLET;
    ee.data.ptr = (void *) data;
    if (epoll_ctl(epoll_fd, op, event_fd, &ee) == -1)
    {
        printf("epoll_ctl(%d, %d) failed", op, event_fd);
        return;
    }
    return;
}

static void del_event_epoll(int epoll_fd, int event_fd)
{
    int op = EPOLL_CTL_DEL;
    if (epoll_ctl(epoll_fd, op, event_fd, NULL) == -1)
    {
        printf("epoll_ctl(%d, %d) failed", op, event_fd);
    }
    return;
}

#endif //_plusplusi_epoll_H
