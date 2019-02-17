/*
 *@author: Jie Feng
 *@desription: this file implements the accept_mutex
 *@date: 19-2-13 上午11:45
 */
#ifndef _plusplusi_accept_mutex_H
#define _plusplusi_accept_mutex_H

#include <cstdio>
#include <pthread.h>
#include <sys/mman.h>

class Accept_Mutex
{
public:
    Accept_Mutex();

    int lock();

    int unlock();

private:
    void init();

private:
    /* actual plusplisi_accept_mutex will be in shared memory */
    pthread_mutex_t *plusplisi_accept_mutex;
};
#endif //_plusplusi_accept_mutex_H
