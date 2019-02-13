#include <iostream>
#include "plusplusi_config.h"
#include <unistd.h>
#include <cstdio>
#include <cctype>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>

#define QUEUE_MAX_COUNT 5
#define BUFF_SIZE 1024

#define SERVER_STRING "Server: plusplusi 0.1.0\r\n"

int run()
{
    int PORT = 1114;
    int WORKER = 4;
    /*const std::string ConfigFile = "../conf/configure.txt";
    Config settings(ConfigFile);

    PORT = settings.Read("port", 1114);
    WORKER = settings.Read("worker", 1);
    std::cout << "port:" << PORT << std::endl;
    std::cout << "workers: " << WORKER << std::endl;*/

    /* 定义server和client的文件描述符 */
    int server_fd(-1);
    int client_fd(-1);

    u_short port = PORT;
    struct sockaddr_in client_addr{};
    struct sockaddr_in server_addr{};
    socklen_t client_addr_len = sizeof(client_addr);

    char buf[BUFF_SIZE];
    char recv_buf[BUFF_SIZE];


    /* 创建一个socket */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        perror("socket");
        exit(-1);
    }
    memset(&server_addr, 0, sizeof(server_addr));
    /* 设置端口，IP，和TCP/IP协议族 */
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    /* 绑定套接字到端口 */
    if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind");
        exit(-1);
    }

    /* 启动socket监听请求，开始等待客户端发来的请求 */
    if (listen(server_fd, QUEUE_MAX_COUNT) < 0)
    {
        perror("listen");
        exit(-1);
    }

    printf("http server running on port %d\n", port);

    int status, i;
    for (i = 0; i < WORKER; i++)
    {
        status = fork();
        if (status == 0 || status == -1) break;
    }
    if (status == -1)
    {
        std::cout << "fork error\n";
        exit(-1);
    } else if (status == 0) //每一个子进程都会运行的代码
    {
        std::cout << "this is worker process: " << getpid() << std::endl;
        while(true)
        {
            /* 调用了accept函数，阻塞了程序，直到接收到客户端的请求 */
            client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &client_addr_len);
            if (client_fd < 0)
            {
                perror("accept");
                exit(-1);
            }
            printf(getpid() + " accept a client\n");
            printf("client socket fd: %d\n", client_fd);

            /* 调用recv函数接收客户端发来的请求信息 */
            //auto hello_len = recv(client_fd, recv_buf, BUFF_SIZE, 0);

            /* 发送响应给客户端 */
            sprintf(buf, "HTTP/1.0 200 OK\r\n");
            send(client_fd, buf, strlen(buf), 0);
            strcpy(buf, SERVER_STRING);
            send(client_fd, buf, strlen(buf), 0);
            sprintf(buf, "Content-Type: text/html\r\n");
            send(client_fd, buf, strlen(buf), 0);
            strcpy(buf, "\r\n");
            send(client_fd, buf, strlen(buf), 0);
            sprintf(buf, "Hello World\r\n");
            send(client_fd, buf, strlen(buf), 0);

            /* 关闭客户端套接字 */
            close(client_fd);
        }
    } else
    {
        std::cout << "this is master process: " << getpid() << std::endl;
        sleep(20);
        close(server_fd);
    }
    return 0;
}