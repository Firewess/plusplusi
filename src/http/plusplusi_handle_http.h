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

typedef std::map<std::string, std::string> HTTP_MAP;
typedef std::map<int, std::string> HTTP_STATUS_CODE;
typedef std::vector<std::pair<std::string, std::string>> HTTP_RESPONSE;

class HTTP_Handler
{
public:
    HTTP_Handler(int serv_sock, int clnt_sock, std::string root, std::string index);

    void operator() (const std::string& str);

private:
    void init_http_head();
    std::string read_file(std::string&& filename);
    void send_to_client(std::string &message);
    std::vector<std::string> split_string(const std::string& str, const std::string& separator);
    inline void close_sock(int sock)
    {
        close(sock);
    }

    std::string map_to_string(const HTTP_MAP& );

private:
    int SERVER_SOCK;
    int CLIENT_SOCK;
    std::string ROOT;
    std::string INDEX;

    int status; //http status code

    std::map<std::string, int> http_method;
    //HTTP_MAP General_HEAD;
    //HTTP_MAP Request_HEAD;
    //HTTP_MAP Response_HEAD;
    //HTTP_MAP Entity_HEAD;

    HTTP_MAP HTTP_Request_Map;   //http request map

    //HTTP_RESPONSE HTTP_Response_Map;
    HTTP_MAP HTTP_Response_Map;

    static HTTP_MAP HTTP_MIME_MAP;
    static HTTP_STATUS_CODE HTTP_STATUS_CODE_MAP;
};

#endif //_plusplusi_function_objects_H_
