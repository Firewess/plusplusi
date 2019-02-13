/*
 *@author: Jie Feng
 *@desription: todo
 *@date: 19-2-13 下午10:49
 */

#include "plusplusi_accept_mutex.h"

Accept_Mutex::Accept_Mutex()
{
    init();
}

void Accept_Mutex::init()
{
    pthread_mutexattr_t mutexattr;
    int fd = open("/dev/zero", O_RDWR, 0);
    if (fd == -1)
        printf("open err :%s\n", strerror(errno));
    plusplisi_accept_mutex = static_cast<pthread_mutex_t *>(mmap(NULL, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE,
                                                                 MAP_SHARED, fd, 0));
    close(fd);
    pthread_mutexattr_init(&mutexattr);
    pthread_mutexattr_setpshared(&mutexattr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(plusplisi_accept_mutex, &mutexattr);
}

void Accept_Mutex::lock()
{
    pthread_mutex_lock(plusplisi_accept_mutex);
}

void Accept_Mutex::unlock()
{
    pthread_mutex_unlock(plusplisi_accept_mutex);
}