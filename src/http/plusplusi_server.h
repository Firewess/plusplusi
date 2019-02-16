/*
 *@author: Jie Feng
 *@desription: this is a test file for server socket
 *@date: 19-2-11 下午11:29
 */
#include <cstdio>
#include <cctype>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <string>

#include "plusplusi_accept_mutex.h"
#include "plusplusi_handle_http.h"
#include "plusplusi_epoll.h"

#define QUEUE_MAX_COUNT 5
#define BUFF_SIZE 1024
#define WORKER_MAX 8
#define SERVER_INFO "plusplusi/0.5 (Linux)\r\n"

class HTTP_SERVER
{
public:
    HTTP_SERVER(int port, std::string&& root, std::string&& index, int worker);
    void run();

private:
    void init_server();
    int create_workers(unsigned int worker_num);
    static void kill_sub_process(int sig);
    static int set_socket_non_blocking(int fd);
    static int proc_receive(struct epoll_event *pstEvent);
    static int proc_accept(struct epoll_event *pstEvent);
    void proc_epoll(int epoll_fd, int timeout);

    void handle_connect(int serv_sock, int pid);

private:
    static int PORT;
    static std::string ROOT;
    static std::string INDEX;
    static int WORKERS;
    static int SERV_SOCK;
    static int workers[WORKER_MAX];
    static int child_epoll_fd;

    static Accept_Mutex accept_mutex;
};
