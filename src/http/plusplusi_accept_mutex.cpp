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
    pthread_mutexattr_t attr;
    int ret;

    //设置互斥量为进程间共享
    plusplisi_accept_mutex = (pthread_mutex_t *) mmap(nullptr, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE,
                                                      MAP_SHARED | MAP_ANON, -1, 0);
    if (MAP_FAILED == plusplisi_accept_mutex)
    {
        perror("mutex mmap failed");
        return;
    }

    //设置attr的属性
    pthread_mutexattr_init(&attr);
    ret = pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    if (ret != 0)
    {
        fprintf(stderr, "mutex set shared failed");
        return;
    }
    pthread_mutex_init(plusplisi_accept_mutex, &attr);
    //printf("accept_mutex_init result:%d\n", pthread_mutex_init(plusplisi_accept_mutex, &attr));
}

int Accept_Mutex::lock()
{
    return pthread_mutex_lock(plusplisi_accept_mutex);
}

int Accept_Mutex::unlock()
{
    return pthread_mutex_unlock(plusplisi_accept_mutex);
}