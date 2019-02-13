/*
 *@author: Jie Feng
 *@desription: this file is only used for function test
 *@date: 19-2-13 下午5:00
 */
#include "../src/http/plusplusi_accept_mutex.h"
#include "../src/http/plusplusi_epoll.h"

int runtest()
{
    int s_s;    /*服务器套接字文件描述符*/
    struct sockaddr_in local;    /*本地地址*/
    signal(SIGINT, sig_int);

    /*建立TCP套接字*/
    s_s = socket(AF_INET, SOCK_STREAM, 0);

    int optval = 1;
    int ret = setsockopt(s_s, SOL_SOCKET, SO_REUSEADDR, (int *) &optval, sizeof(optval));

    /*初始化地址接哦股*/
    memset(&local, 0, sizeof(local));/*清零*/
    local.sin_family = AF_INET;/*AF_INET协议族*/
    local.sin_addr.s_addr = htonl(INADDR_ANY);/*任意本地地址*/
    local.sin_port = htons(SERVER_PORT);/*服务器端口*/

    /*将套接字文件描述符绑定到本地地址和端口*/
    int err = bind(s_s, (struct sockaddr *) &local, sizeof(local));
    err = listen(s_s, BACKLOG);/*侦听*/

    //init the write lock
    accept_mutex_init();

    /*处理客户端连接*/
    pid_t pid[PIDNUMB];
    int i = 0;
    for (i = 0; i < PIDNUMB; i++)
    {
        pid[i] = fork();
        if (pid[i] == 0)/*子进程*/
        {
            handle_connect(s_s, getpid());
        }
        else sleep(30);
    }

    close(s_s);

    return 0;
}

int main()
{
    runtest();

    return 0;
}