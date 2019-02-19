/*
 *@author: Jie Feng
 *@desription: todo
 *@date: 19-2-16 上午11:14
 */
#include <unistd.h>
#include <iostream>
#include <sys/epoll.h>

class Test
{
public:
    static int id;
};

int Test::id = 0;

int test1()
{
    Test test;
    int status, i;
    for (i = 0; i < 2; i++)
    {
        status = fork();
        if (status == 0 || status == -1) break;
    }
    if (status == -1)
    {
        //error
    } else if (status == 0)
    {
        printf("pid: %d\n", getpid());
        int epoll_fd = epoll_create(1);
        printf("%d 's epoll fd: %d\n", getpid(), epoll_fd);
        close(epoll_fd);
    } else
    {
        sleep(10);
    }
    return 0;
}