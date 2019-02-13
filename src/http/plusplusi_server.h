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

#define QUEUE_MAX_COUNT 5
#define BUFF_SIZE 1024

#define SERVER_STRING "Server: plusplusi 0.1.0\r\n"

class HTTP_SERVER
{
public:
    HTTP_SERVER(int port, std::string&& root, std::string&& index, int worker);

    void handle_connect(int serv_sock, int pid);

private:
    void init_server();

    void sig_int(int num)
    {
        exit(1);
    }

private:
    int PORT;
    std::string ROOT;
    std::string INDEX;
    int WORKERS;
    int SERV_SOCK;

    Accept_Mutex accept_mutex;
};
