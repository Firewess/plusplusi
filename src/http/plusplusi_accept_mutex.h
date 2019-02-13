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

#define SERVER_PORT 1114
#define BACKLOG 5
#define PIDNUMB 2

//https://blog.csdn.net/gettogetto/article/details/77430852
class Accept_Mutex
{
public:
    Accept_Mutex();

    void lock();

    void unlock();

private:
    void init();

private:
    /* actual plusplisi_accept_mutex will be in shared memory */
    pthread_mutex_t *plusplisi_accept_mutex;
};
#endif //_plusplusi_accept_mutex_H
