/*
 *@author: Jie Feng
 *@desription: todo
 *@date: 19-2-13 下午7:54
 */
#include <sys/socket.h>
#include "plusplusi_handle_http.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstring>
#include <ctime>
#include <cmath>

HTTP_MAP HTTP_Handler::HTTP_MIME_MAP = {
        std::make_pair("html", "text/html"),
        std::make_pair("xml", "text/xml"),
        std::make_pair("xhtml", "application/xhtml+xml"),
        std::make_pair("txt", "text/plain"),
        std::make_pair("rtf", "application/rtf"),
        std::make_pair("pdf", "application/pdf"),
        std::make_pair("doc", "application/msword"),
        std::make_pair("png", "image/png"),
        std::make_pair("gif", "image/gif"),
        std::make_pair("jpeg", "image/jpeg"),
        std::make_pair("jpg", "image/jpeg"),
        std::make_pair("au", "audio/basic"),
        std::make_pair("avi", "video/avi"),
        std::make_pair("ico", "image/x-icon"),
        std::make_pair("js", "application/x-javascript"),
        std::make_pair("htm", "text/html"),
        std::make_pair("mp4", "video/mpeg4"),
        std::make_pair("mp3", "audio/mp3"),
        std::make_pair("css", "text/css")
};

HTTP_STATUS_CODE HTTP_Handler::HTTP_STATUS_CODE_MAP = {
        std::make_pair(200, "OK"),
        std::make_pair(400, "Bad Request"),
        std::make_pair(401, "Unauthorized"),
        std::make_pair(403, "Forbidden"),
        std::make_pair(404, "Not Found"),
        std::make_pair(301, "Moved Permanently"),
        std::make_pair(304, "Not Modified"),
        std::make_pair(503, "Service Unavailable")
};

HTTP_Handler::HTTP_Handler(int serv_sock, int clnt_sock, std::string root, std::string index, std::string server)
        : SERVER_SOCK(serv_sock), CLIENT_SOCK(clnt_sock), ROOT(root), INDEX(index), SERVER_INFO(server)
{
    http_method.insert(std::make_pair("GET", GET));
    http_method.insert(std::make_pair("POST", POST));
    http_method.insert(std::make_pair("HEAD", HEAD));
    http_method.insert(std::make_pair("PUT", PUT));
    http_method.insert(std::make_pair("DELETE", DELETE));
    http_method.insert(std::make_pair("CONNECT", CONNECT));
    http_method.insert(std::make_pair("OPTIONS", OPTIONS));
    http_method.insert(std::make_pair("TRACE", TRACE));
    http_method.insert(std::make_pair("PATCH", PATCH));
    mmap_start_addr = nullptr;
    //init_http_head();
}

void HTTP_Handler::operator()(const std::string &str)
{
    auto heads = split_string(str, "\r\n");
    auto request_line = split_string(heads.at(0), " ");
    auto uri = request_line.at(1);
    for (const auto head : heads)
    {
        auto temp = split_string(head, ": ");
        if (temp.size() == 2)
        {
            HTTP_Request_Map.insert(std::make_pair(std::move(temp.at(0)), std::move(temp.at(1))));
        }
    }
    switch (http_method[request_line.at(0)])
    {
        case GET:
        {
            std::string path;
            std::string suffix;
            HTTP_Response_Map.insert(std::move(
                    std::make_pair(std::move(std::string("Server")), SERVER_INFO)));
            if (uri == "/")
            {
                path = ROOT + "/" + INDEX;
                suffix = split_string(INDEX, ".").at(1);
            } else
            {
                auto temp = split_string(uri, ".");
                suffix = temp.at(temp.size() - 1);
                path = ROOT + std::move(uri); //move semantic
            }
            if (HTTP_MIME_MAP.find(suffix) != HTTP_MIME_MAP.end())
            {
                //if find this MIME type file, return its MIME
                HTTP_Response_Map.insert(std::move(std::make_pair(std::string("Content-Type"), HTTP_MIME_MAP[suffix])));
            } else
            {
                //else, handle it as plain text
                HTTP_Response_Map.insert(std::move(
                        std::make_pair(std::string("Content-Type"), std::move(std::string("text/plain")))));
                suffix = "txt";
            }
            //response = std::move(read_file(std::move(path)));

            //mmap_start_addr = memory_mapping(path);

            std::string response = std::move(read_file(std::move(path)));

            //HTTP_Response_Map.insert(std::make_pair(std::move(std::string("Content-Length")),
            //                                        std::move(std::to_string(file_info.st_size))));

            HTTP_Response_Map.insert(std::make_pair(std::move(std::string("Content-Length")),
                                                    std::move(std::to_string(response.length()))));
            std::string res_head =
                    request_line.at(2) + " " + std::to_string(status) + " " + HTTP_STATUS_CODE_MAP[status] + "\r\n";
            std::string field = std::move(map_to_string(HTTP_Response_Map));
            str_http_res_header = std::move(res_head) + std::move(field) + std::move(response);
            HTTP_Response_Map.clear();
            break;
        }
        case POST:
            break;
        case HEAD:
            break;
        case PUT:
            break;
        case DELETE:
            break;
        case CONNECT:
            break;
        case OPTIONS:
            break;
        case TRACE:
            break;
        case PATCH:
            break;
        default:
            return;
    }
}

std::string HTTP_Handler::read_file(std::string &&filename)
{
    std::ifstream is(filename.c_str(), std::ifstream::in | std::ios::binary);
    status = 503;
    if (!is)
    {
        status = 404;
        return "";
    } else
    {
        if (HTTP_Request_Map.find("If-Modified-Since") != HTTP_Request_Map.end())
        {
            stat(filename.data(), &file_info);
            struct tm mod_time;
            if (strptime(HTTP_Request_Map["If-Modified-Since"].data(), "%a, %d %b %Y %H:%M:%S GMT", &mod_time) ==
                (char *) nullptr)
            {
                return "";
            }
            time_t client_time = mktime(&mod_time);
            double time_diff = difftime(file_info.st_mtime, client_time);
            if (fabs(time_diff) < 1e-6)
            {
                status = 304;
                return "";
            }
        }
        is.seekg(0, is.end);
        auto file_length = is.tellg();
        is.seekg(0, is.beg);
        auto *buffer = new char[file_length];
        is.read(buffer, file_length);
        std::string image(buffer, file_length);
        status = 200;
        delete[] buffer;
        return image;
    }
}

char *HTTP_Handler::memory_mapping(std::string &filename)
{
    if (stat(filename.data(), &file_info) < 0)
    {
        status = 404;
        return nullptr;
    }
    int srcfd = open(filename.data(), O_RDONLY, 0);
    // can use sendfile
    mmap_start_addr = static_cast<char *>(mmap(NULL, file_info.st_size, PROT_READ, MAP_PRIVATE, srcfd, 0));
    close(srcfd);
    if (mmap_start_addr == (void *) -1)
    {
        status = 500;
        mmap_start_addr = nullptr;
    }
    status = 200;
    return mmap_start_addr;
}

int HTTP_Handler::send_to_client(std::string &message)
{
    //send(CLIENT_SOCK, message.data(), message.size(), 0);
    auto body_length = message.size();
    auto *buffer = new char[body_length];
    memcpy(buffer, message.data(), body_length);
    int ret = write(CLIENT_SOCK, buffer, body_length);
    delete[] buffer;
    return ret;
}

int HTTP_Handler::send_to_client(void *usrbuf, size_t n)
{
    size_t left = n;
    ssize_t written;
    char *bufp = (char *) usrbuf;
    while (left > 0)
    {
        if ((written = write(CLIENT_SOCK, bufp, left)) <= 0)
        {
            if (errno == EINTR)  /* interrupted by sig handler return */
                written = 0;    /* and call write() again */
            else
            {
                return -1;       /* errorno set by write() */
            }
        }
        left -= written;
        bufp += written;
    }
    return n;
}

void HTTP_Handler::memory_unmapping()
{
    munmap(mmap_start_addr, file_info.st_size);
    mmap_start_addr = nullptr;
}

/*
 *@desription: 辅助函数，将字符串按照分隔符进行分割
 *@date: 19-2-11 下午11:29
 */
std::vector<std::string> HTTP_Handler::split_string(const std::string &str, const std::string &separator)
{
    std::vector<std::string> sub_strings;
    std::string::size_type pos_substr_start;

    std::string::size_type pos_separator;
    pos_separator = str.find(separator);
    pos_substr_start = 0;
    while (std::string::npos != pos_separator)
    {
        sub_strings.push_back(str.substr(pos_substr_start, pos_separator - pos_substr_start));
        pos_substr_start = pos_separator + separator.size();
        pos_separator = str.find(separator, pos_substr_start);
    }
    if (pos_substr_start != str.length())
        sub_strings.push_back(str.substr(pos_substr_start));
    return sub_strings;
}

std::string HTTP_Handler::map_to_string(const HTTP_MAP &the_map)
{
    std::string result;
    for (const auto &t: the_map)
        result += t.first + ": " + t.second + "\r\n";
    result += "\r\n";
    return result;
}

void HTTP_Handler::do_error()
{
    std::string res_head = "HTTP/1.1 " + std::to_string(status) + " " + HTTP_STATUS_CODE_MAP[status] + "\r\n";
    std::string response = "<html><title>plusplusi error</title>\n<body bgcolor=\"\"ffffff\"\"><center><h1>" +
                           std::to_string(status) + ": " + HTTP_STATUS_CODE_MAP[status] +
                           "</h1>\n<hr><em>plusplusi web server</em></center>\n</body></html>";
    HTTP_Response_Map.clear();
    HTTP_Response_Map.insert(std::move(
            std::make_pair(std::move(std::string("Server")), SERVER_INFO)));
    HTTP_Response_Map.insert(std::move(
            std::make_pair(std::string("Content-Type"), std::move(std::string("text/html")))));
    HTTP_Response_Map.insert(std::move(
            std::make_pair(std::string("Connection"), std::move(std::string("close")))));
    HTTP_Response_Map.insert(std::move(
            std::make_pair(std::string("Content-length"), std::move(std::to_string(response.length())))));

    std::string temp = std::move(res_head) + std::move(map_to_string(HTTP_Response_Map)) + std::move(response);

    send_to_client(temp);
    HTTP_Response_Map.clear();
}
