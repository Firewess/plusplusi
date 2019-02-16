/*
 *@author: Jie Feng
 *@desription: todo
 *@date: 19-2-13 下午8:13
 */
#include "plusplusi_server.h"
#include <iostream>

Accept_Mutex HTTP_SERVER::accept_mutex;
int HTTP_SERVER::PORT = 1114;
std::string HTTP_SERVER::ROOT = "html";
std::string HTTP_SERVER::INDEX = "index.html";
int HTTP_SERVER::WORKERS = 4;
int HTTP_SERVER::SERV_SOCK = -1;
int HTTP_SERVER::workers[WORKER_MAX];
int HTTP_SERVER::child_epoll_fd = -1;

HTTP_SERVER::HTTP_SERVER(int port, std::string &&root, std::string &&index, int worker)
{
    PORT = port;
    ROOT = root;
    INDEX = index;
    WORKERS = worker;
    init_server();
}

void HTTP_SERVER::init_server()
{
    struct sockaddr_in server_addr{};
    unsigned value = 1;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);
    if ((SERV_SOCK = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        return;
    }
    setsockopt(SERV_SOCK, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value));
    if (bind(SERV_SOCK, (struct sockaddr *) &server_addr, sizeof(server_addr)))
    {
        printf("bind socket error: %s(errno: %d)\n", strerror(errno), errno);
        return;
    }
    if (listen(SERV_SOCK, 10) == -1)
    {
        printf("listen socket error: %s(errno: %d)\n", strerror(errno), errno);
        return;
    }
    std::cout << "server is running on " << PORT << std::endl;

    //handle_connect(SERV_SOCK, getpid());
    //close(SERV_SOCK);
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
        HTTP_Handler http_handler(SERV_SOCK, clnt_sock, ROOT, INDEX, SERVER_INFO);

        char buff[BUFF_SIZE];/*收发数据缓冲区*/
        memset(buff, 0, BUFF_SIZE);/*清零*/

        auto hello_len = recv(clnt_sock, buff, BUFF_SIZE, 0);

        //parse http request message
        http_handler(buff);
    }
}

int HTTP_SERVER::create_workers(unsigned int worker_num)
{
    unsigned int i;
    unsigned int real_num = worker_num;
    int pid;
    if (real_num > WORKER_MAX)
    {
        real_num = WORKER_MAX;
    }
    for (i = 0; i < real_num; i++)
    {
        pid = fork();
        if (0 == pid)
        {
            return 0;
        } else if (0 < pid)
        {
            workers[i] = pid;
            continue;
        } else
        {
            printf("fork error\r\n");
            return 0;
        }
    }
    return 1;
}

void HTTP_SERVER::kill_sub_process(int sig)
{
    for (int i = 0; i < WORKERS; i++)
    {
        //kill sub process
        kill(workers[i], SIGTERM);
    }
}

int HTTP_SERVER::set_socket_non_blocking(int fd)
{
    int flags, s;
    flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
    {
        perror("fcntl");
        return -1;
    }
    flags |= O_NONBLOCK;
    s = fcntl(fd, F_SETFL, flags);
    if (s == -1)
    {
        perror("fcntl");
        return -1;
    }
    return 0;
}

int HTTP_SERVER::proc_receive(struct epoll_event *pstEvent)
{
    char buff[4096];   /* 缓存 */
    int len;
    Epoll_Data_S *data = (Epoll_Data_S *) (pstEvent->data.ptr);
    int epoll_fd = data->Epoll_FD;
    int event_fd = data->Event_FD;
    if (pstEvent->events & EPOLLIN)
    {
        HTTP_Handler http_handler(SERV_SOCK, event_fd, ROOT, INDEX, SERVER_INFO);
        while (true)
        {
            /* 读取数据 */
            len = (int) recv(event_fd, buff, sizeof(buff), 0);

            //handle http request message
            http_handler(buff);

            if (len <= 0)
            {
                if (errno == EINTR)
                {
                    continue;
                }
                del_event_epoll(epoll_fd, event_fd);
                close(event_fd);
                free(data);
            } else if (len > 0)
            {
                buff[len] = '\0';
                //usleep(10000);
            }
            break;
        }
    } else if (pstEvent->events & EPOLLHUP)
    {
        printf("receive EPOLLHUP or EPOLLOUT\r\n");
        del_event_epoll(epoll_fd, event_fd);
        close(event_fd);
        free(data);
    } else
    {
        // printf("receive others pstEvent->events=%d\r\n", pstEvent->events);
    }
    return 0;
}

int HTTP_SERVER::proc_accept(struct epoll_event *pstEvent)
{
    int client_fd;
    Epoll_Data_S *data = (Epoll_Data_S *) (pstEvent->data.ptr);
    int epoll_fd = data->Epoll_FD;
    int event_fd = data->Event_FD;
    if (accept_mutex.lock() == 0)
    {
        while (-1 != (client_fd = accept(event_fd, (struct sockaddr *) NULL, NULL)))
        {
            set_socket_non_blocking(client_fd);
            add_event_epoll(child_epoll_fd, client_fd, proc_receive);
        }
        accept_mutex.unlock();
    }
    return 0;
}

void HTTP_SERVER::proc_epoll(int epoll_fd, int timeout)
{
    int event_num;
    int i;
    struct epoll_event events[EVENT_LIST_MAX];
    event_num = epoll_wait(epoll_fd, events, EVENT_LIST_MAX, timeout);
    for (i = 0; i < event_num; i++)
    {
        Epoll_Data_S *data = (Epoll_Data_S *) (events[i].data.ptr);
        data->callback_fun(&(events[i]));
    }
}

void HTTP_SERVER::run()
{
    set_socket_non_blocking(SERV_SOCK);
    int epoll_fd = create_epoll(EVENT_MAX);
    if (-1 == epoll_fd)
    {
        close(epoll_fd);
        return;
    }

    /* 将监听端口加到epoll */
    add_event_epoll(epoll_fd, SERV_SOCK, proc_accept);

    /* 创建子进程  */
    int master = create_workers(WORKERS);
    /* 主进程 */
    if (master)
    {
        while (true)
        {
            signal(SIGTERM, kill_sub_process);
            pause();
        }
    } else
    {
        /* 子进程创建epoll */
        child_epoll_fd = create_epoll(EVENT_MAX);
        if (-1 == child_epoll_fd)
        {
            close(SERV_SOCK);
            return;
        }
        while (true)
        {
            /* 处理父epoll消息 */
            proc_epoll(epoll_fd, 500);

            /* 处理子epoll消息 */
            proc_epoll(child_epoll_fd, 500);
        }
    }
}