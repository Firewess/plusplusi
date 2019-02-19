/*
 *@author: Jie Feng
 *@desription: this file encapsulates epoll-related functions & struct
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

class HTTP_Handler;
class Timer;

typedef struct EPOLL_DATA_S
{
    int Epoll_FD;
    int Event_FD;
    HTTP_Handler* http_handler;
    Timer* timer;
    CALLBACK_FUN callback_fun;
} Epoll_Data_S;

static int create_epoll(unsigned int event_num)
{
    int epoll_fd;
    epoll_fd = epoll_create(event_num);
    if (-1 == epoll_fd)
    {
        printf("epoll create failed\n");
        return -1;
    }
    return epoll_fd;
}

static void add_event_epoll(int epoll_fd, int event_fd, HTTP_Handler* http_handler, CALLBACK_FUN callback_fun)
{
    int op = EPOLL_CTL_ADD;
    struct epoll_event ee{};
    memset(&ee, 0, sizeof(ee));
    auto *data = new Epoll_Data_S();
    if (data == nullptr)
    {
        return;
    }
    data->Epoll_FD = epoll_fd;
    data->Event_FD = event_fd;
    data->http_handler = http_handler;
    data->timer = nullptr;
    data->callback_fun = callback_fun;
    ee.events = EPOLLIN | EPOLLOUT | EPOLLHUP;
    //ee.events = EPOLLIN|EPOLLET;
    ee.data.ptr = (void *) data;
    if (epoll_ctl(epoll_fd, op, event_fd, &ee) == -1)
    {
        printf("add_event_epoll(%d, %d) failed: %s\n", op, event_fd, strerror(errno));
    }
}

static void mod_event_epoll(int epoll_fd, int event_fd, struct epoll_event *event)
{
    if(epoll_ctl(epoll_fd, EPOLL_CTL_MOD, event_fd, event) == -1)
    {
        printf("mod_event_epoll(%d, %d) failed: %s\n", EPOLL_CTL_MOD, event_fd, strerror(errno));
    }
}

static void del_event_epoll(int epoll_fd, int event_fd)
{
    int op = EPOLL_CTL_DEL;
    if (epoll_ctl(epoll_fd, op, event_fd, nullptr) == -1)
    {
        printf("del_event_epoll(%d, %d) failed: %s\n", op, event_fd, strerror(errno));
    }
}

#endif //_plusplusi_epoll_H
