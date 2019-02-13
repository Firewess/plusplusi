/*
 *@author: Jie Feng
 *@desription: this file implements some function objects used for parse http
 *@date: 19-2-12 上午10:32
 */

#ifndef _plusplusi_function_objects_H_
#define _plusplusi_function_objects_H_

#include <string>
#include <map>
#include <vector>
#include <unistd.h>

#define GET 0
#define POST 1
#define HEAD 2
#define PUT 3
#define DELETE 4
#define CONNECT 5
#define OPTIONS 6
#define TRACE 7
#define PATCH 8

typedef std::map<std::string, std::string> HTTP_HEAD_MAP;

class HTTP_Handler
{
public:
    HTTP_Handler(int serv_sock, int clnt_sock, std::string root, std::string index);

    void operator() (const std::string& str);

private:
    void init_http_head();
    std::string& read_file(std::string&& filename);
    void send_to_client(std::string &message);
    std::vector<std::string> split_string(const std::string& str, const std::string& separator);
    inline void close_sock(int sock)
    {
        close(sock);
    }

private:
    int SERV_SOCK;
    int CLNT_SOCK;
    std::string ROOT;
    std::string INDEX;

    int status; //http status code

    std::map<std::string, int> http_method;
    HTTP_HEAD_MAP General_HEAD;
    HTTP_HEAD_MAP Request_HEAD;
    HTTP_HEAD_MAP Response_HEAD;
    HTTP_HEAD_MAP Entity_HEAD;
};

#endif //_plusplusi_function_objects_H_
