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
#include <sys/stat.h>   //include this header to access a file's information through filename
#include <fcntl.h>
#include <sys/mman.h> //for memory mapping

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

class HTTP_SERVER;

class HTTP_Handler
{
    friend class HTTP_SERVER;
public:
    HTTP_Handler(int serv_sock, int clnt_sock, std::string root, std::string index, std::string server);

    void operator() (const std::string& str);

private:
    std::string read_file(std::string&& filename);

    char* memory_mapping(std::string& filename);
    int send_to_client(std::string &message);
    int send_to_client(void *usrbuf, size_t n);
    void memory_unmapping();

    std::vector<std::string> split_string(const std::string& str, const std::string& separator);
    inline void close_sock(int sock)
    {
        close(sock);
    }

    std::string map_to_string(const HTTP_MAP& );
    void do_error();

private:
    int SERVER_SOCK;
    int CLIENT_SOCK;
    std::string ROOT;
    std::string INDEX;
    std::string SERVER_INFO;

    int status; //http status code
    std::string str_http_res_header;

    char *mmap_start_addr;
    struct stat file_info;

    std::map<std::string, int> http_method;

    HTTP_MAP HTTP_Request_Map;   //http request map

    //HTTP_RESPONSE HTTP_Response_Map;
    HTTP_MAP HTTP_Response_Map;

    static HTTP_MAP HTTP_MIME_MAP;
    static HTTP_STATUS_CODE HTTP_STATUS_CODE_MAP;
};

#endif //_plusplusi_function_objects_H_
