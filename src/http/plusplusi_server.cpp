/*
 *@author: Jie Feng
 *@desription: todo
 *@date: 19-2-13 下午8:13
 */
#include "plusplusi_server.h"
#include <iostream>

HTTP_SERVER::HTTP_SERVER(int port, std::string &&root, std::string &&index, int worker)
        : PORT(port), ROOT(root), INDEX(index), WORKERS(worker), accept_mutex(Accept_Mutex())
{
    init_server();
}

void HTTP_SERVER::init_server()
{
    struct sockaddr_in server_addr{};    /*本地地址*/
    //signal(SIGINT, sig_int);

    /*建立TCP套接字*/
    SERV_SOCK = socket(AF_INET, SOCK_STREAM, 0);
    int optval = 1;
    int ret = setsockopt(SERV_SOCK, SOL_SOCKET, SO_REUSEADDR, (int *) &optval, sizeof(optval));

    /*初始化地址*/
    memset(&server_addr, 0, sizeof(server_addr));/*清零*/
    server_addr.sin_family = AF_INET;/*AF_INET协议族*/
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);/*任意本地地址*/
    server_addr.sin_port = htons(SERVER_PORT);/*服务器端口*/

    /*将套接字文件描述符绑定到本地地址和端口*/
    if (bind(SERV_SOCK, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind");
        exit(1);
    }
    if (listen(SERV_SOCK, BACKLOG) < 0)
    {
        perror("listen");
        exit(2);
    }
    std::cout << "server is running on 1114\n";

    /*处理客户端连接*/
    /*int status, i;
    for (i = 0; i < WORKERS; i++)
    {
        status = fork();
        if (status == 0 || status == -1) break;
    }
    if (status == -1)
    {
        std::cout << "fork error\n";
        exit(-1);
    }else if (status == 0) //每一个子进程都会运行的代码
    {
        std::cout << "this is worker process: " << getpid() << std::endl;
        handle_connect(SERVER_SOCK, getpid());
    } else
    {
        std::cout << "this is master process: " << getpid() << std::endl;
        sleep(20);
    }*/

    handle_connect(SERV_SOCK, getpid());
    close(SERV_SOCK);
}

void HTTP_SERVER::handle_connect(int serv_sock, int pid)
{
    int clnt_sock;    /*客户端套接字文件描述符*/
    struct sockaddr_in clnt_addr{};    /*客户端地址*/
    socklen_t clnt_addr_len = sizeof(clnt_addr);
    /*主处理过程*/
    while (true)
    {
        /*接收客户端连接*/
        accept_mutex.lock(); //add write lock
        clnt_sock = accept(serv_sock, (struct sockaddr *) &clnt_addr, &clnt_addr_len);
        accept_mutex.unlock(); //release write lock
        if (clnt_sock > 0)
            printf("pid[%d] accept a client[%d]\n", pid, clnt_sock);
        else
            printf("pid[%d] accept fail\n", pid);
        HTTP_Handler http_handler(SERV_SOCK, clnt_sock, ROOT, INDEX);

        char buff[BUFF_SIZE];/*收发数据缓冲区*/
        memset(buff, 0, BUFF_SIZE);/*清零*/

        auto hello_len = recv(clnt_sock, buff, BUFF_SIZE, 0);

        std::cout<<"recv: "<<buff<<std::endl;
        //parse http request message
        http_handler(buff);
    }
}