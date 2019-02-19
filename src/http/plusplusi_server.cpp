/*
 *@author: Jie Feng
 *@desription: todo
 *@date: 19-2-13 下午8:13
 */
#include "plusplusi_server.h"
#include <iostream>

Accept_Mutex *HTTP_SERVER::accept_mutex = new Accept_Mutex();
int HTTP_SERVER::PORT = 1114;
std::string HTTP_SERVER::ROOT = "html";
std::string HTTP_SERVER::INDEX = "index.html";
int HTTP_SERVER::WORKERS = 4;
int HTTP_SERVER::SERV_SOCK = -1;
int HTTP_SERVER::workers[WORKER_MAX];
int HTTP_SERVER::child_epoll_fd = -1;
TimerManager HTTP_SERVER::timer_manager;

HTTP_SERVER::HTTP_SERVER(int port, std::string &&root, std::string &&index, int worker)
{
    PORT = port;
    ROOT = root;
    INDEX = index;
    WORKERS = worker;
    init_server();
}

HTTP_SERVER::~HTTP_SERVER()
{
    if (accept_mutex != nullptr)
    {
        delete accept_mutex;
        accept_mutex = nullptr;
    }
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
        } else if (pid > 0)
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

int HTTP_SERVER::proc_receive(struct epoll_event *ready_event)
{
    char buff[4096];   /* 缓存 */
    memset(buff, 0, sizeof(buff));
    int len;
    auto *data = (Epoll_Data_S *) (ready_event->data.ptr);
    int epoll_fd = data->Epoll_FD;
    int event_fd = data->Event_FD;
    auto http_handler = data->http_handler;
    if (ready_event->events & EPOLLIN)
    {
        while (true)
        {
            /* 读取数据 */
            len = (int) recv(event_fd, buff, sizeof(buff), 0);
            if (len <= 0)
            {
                if (errno == EINTR || errno == EAGAIN)
                {
                    continue;
                }
                close_fd(data);
            } else if (len > 0)
            {
                //handle http request message
                (*http_handler)(buff);

                if (http_handler->HTTP_Request_Map.find("Connection") != http_handler->HTTP_Request_Map.end() &&
                    http_handler->HTTP_Request_Map["Connection"] == "keep-alive" && data->timer == nullptr)
                {
                    Timer* timer = new Timer(timer_manager);
                    timer->start(&HTTP_SERVER::close_fd, data, TIME_OUT);
                }

                ready_event->events = EPOLLOUT | EPOLLET;
                ready_event->data.ptr = data;
                mod_event_epoll(epoll_fd, event_fd, ready_event);
            }
            break;
        }
    } else if (ready_event->events & EPOLLOUT)
    {
        if (!http_handler->str_http_res_header.empty())
        {
            //printf("%d 's %d receive EPOLLOUT & data is ready\r\n", getpid(), event_fd);
            if(http_handler->status < 400)
            {
                http_handler->send_to_client(http_handler->str_http_res_header);
                http_handler->str_http_res_header.clear();
            } else
            {
                http_handler->str_http_res_header.clear();
                http_handler->do_error();
            }

            /*if(http_handler->mmap_start_addr != nullptr)
            {
                http_handler->send_to_client(http_handler->mmap_start_addr, http_handler->file_info.st_size);
            }
            http_handler->memory_unmapping();*/

            if (http_handler->HTTP_Request_Map.find("Connection") == http_handler->HTTP_Request_Map.end() ||
                http_handler->HTTP_Request_Map["Connection"] != "keep-alive")
            {
                close_fd(data);
            }
        } else
        {
            //printf("%d 's %d receive EPOLLOUT but data is not ready\r\n", getpid(), event_fd);
        }
    } else
    {
        printf("receive others ready_event->events=%d\r\n", ready_event->events);
    }
    return 0;
}

int HTTP_SERVER::proc_accept(struct epoll_event *ready_event)
{
    int client_fd;
    struct sockaddr_in client_addr{};
    socklen_t client_addr_len = sizeof(client_addr);
    Epoll_Data_S *data = (Epoll_Data_S *) (ready_event->data.ptr);
    int event_fd = data->Event_FD;
    if (accept_mutex->lock() == 0)
    {
        while (-1 != (client_fd = accept(event_fd, (struct sockaddr *) nullptr, nullptr)))
        {
            //printf("%d accept success: addr %d, port %d\n", getpid(), client_addr.sin_addr.s_addr, client_addr.sin_port);
            HTTP_Handler *http_handler = new HTTP_Handler(SERV_SOCK, client_fd, ROOT, INDEX, SERVER_INFO);
            set_socket_non_blocking(client_fd);
            add_event_epoll(child_epoll_fd, client_fd, http_handler, proc_receive);
        }
        accept_mutex->unlock();
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
        auto *data = (Epoll_Data_S *) (events[i].data.ptr);
        data->callback_fun(&(events[i]));
    }
}

void HTTP_SERVER::close_fd(Epoll_Data_S *ptr)
{
    if(ptr->timer != nullptr)
    {
        ptr->timer->stop();
        delete ptr->timer;
    }
    delete ptr->http_handler;
    del_event_epoll(ptr->Epoll_FD, ptr->Event_FD);
    close(ptr->Event_FD);
    delete ptr;
    //std::cout << getpid() << ": " << ptr->Event_FD << " closed because of time out\n";
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
    add_event_epoll(epoll_fd, SERV_SOCK, nullptr, proc_accept);

    /* 创建子进程  */
    int master = create_workers(WORKERS);
    /* 主进程 */
    if (master)
    {
        //TimerManager timer_manager();
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
            timer_manager.work_cycle();

            /* 处理父epoll消息 */
            proc_epoll(epoll_fd, 500);

            /* 处理子epoll消息 */
            proc_epoll(child_epoll_fd, 500);
        }
    }
}
