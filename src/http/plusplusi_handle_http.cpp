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
    //init_http_head();
}

/*void HTTP_Handler::init_http_head()
{
    General_HEAD.insert(std::make_pair("Cache-Control", "")); //控制缓存的行为
    General_HEAD.insert(std::make_pair("Connection", "")); //控制不再转发给代理的首部字段、管理持久连接
    General_HEAD.insert(std::make_pair("Date", "")); //创建报文的日期时间
    General_HEAD.insert(std::make_pair("Pragma", "")); //报文指令
    General_HEAD.insert(std::make_pair("Trailer", "")); //报文末端的首部一览
    General_HEAD.insert(std::make_pair("Transfer-Encoding", "")); //指定报文主体的传输编码方式
    General_HEAD.insert(std::make_pair("Upgrade", "")); //升级为其他协议
    General_HEAD.insert(std::make_pair("Via", "")); //代理服务器的相关信息
    General_HEAD.insert(std::make_pair("Warning", "")); //错误通知

    Request_HEAD.insert(std::make_pair("Accept", "")); //用户代理可处理的媒体类型
    Request_HEAD.insert(std::make_pair("Accept-Charset", "")); //优先的字符集
    Request_HEAD.insert(std::make_pair("Accept-Encoding", "")); //优先的内容编码
    Request_HEAD.insert(std::make_pair("Accept-Language", "")); //优先的语言(自然语言)
    Request_HEAD.insert(std::make_pair("Authorization", "")); //Web 认证信息
    Request_HEAD.insert(std::make_pair("Expect", "")); //期待服务器的特定行为
    Request_HEAD.insert(std::make_pair("From", "")); //用户的电子邮箱地址
    Request_HEAD.insert(std::make_pair("Host", "")); //请求资源所在服务器
    Request_HEAD.insert(std::make_pair("If-Match", "")); //比较实体标记(ETag)
    Request_HEAD.insert(std::make_pair("If-Modified-Since", "")); //比较资源的更新时间
    Request_HEAD.insert(std::make_pair("If-None-Match", "")); //比较实体标记(与 If-Match 相反)
    Request_HEAD.insert(std::make_pair("If-Range", "")); //资源未更新时发送实体 Byte 的范围请求
    Request_HEAD.insert(std::make_pair("If-Unmodified-Since", "")); //比较资源的更新时间(与 If-Modified-Since 相反)
    Request_HEAD.insert(std::make_pair("Max-Forwards", "")); //最大传输逐跳数
    Request_HEAD.insert(std::make_pair("Proxy-Authorization", "")); //代理服务器要求客户端的认证信息
    Request_HEAD.insert(std::make_pair("Range", "")); //实体的字节范围请求
    Request_HEAD.insert(std::make_pair("Referer", "")); //对请求中 URI 的原始获取方
    Request_HEAD.insert(std::make_pair("TE", "")); //传输编码的优先级
    Request_HEAD.insert(std::make_pair("User-Agent", "")); //HTTP 客户端程序的信息

    Response_HEAD.insert(std::make_pair("Accept-Ranges", "")); //是否接受字节范围请求
    Response_HEAD.insert(std::make_pair("Age", "")); //推算资源创建经过时间
    Response_HEAD.insert(std::make_pair("ETag", "")); //资源的匹配信息
    Response_HEAD.insert(std::make_pair("Location", "")); //令客户端重定向至指定 URI
    Response_HEAD.insert(std::make_pair("Proxy-Authenticate", "")); //代理服务器对客户端的认证信息
    Response_HEAD.insert(std::make_pair("Retry-After", "")); //对再次发起请求的时机要求
    Response_HEAD.insert(std::make_pair("Server", "")); //HTTP 服务器的安装信息
    Response_HEAD.insert(std::make_pair("Vary", "")); //代理服务器缓存的管理信息
    Response_HEAD.insert(std::make_pair("WWW-Authenticate", "")); //服务器对客户端的认证信息

    Entity_HEAD.insert(std::make_pair("Allow", "")); //资源可支持的 HTTP 方法
    Entity_HEAD.insert(std::make_pair("Content-Encoding", "")); //实体主体适用的编码方式
    Entity_HEAD.insert(std::make_pair("Content-Language", "")); //实体主体的自然语言
    Entity_HEAD.insert(std::make_pair("Content-Length", "")); //实体主体的大小
    Entity_HEAD.insert(std::make_pair("Content-Location", "")); //替代对应资源的 URI
    Entity_HEAD.insert(std::make_pair("Content-MD5", "")); //实体主体的报文摘要
    Entity_HEAD.insert(std::make_pair("Content-Range", "")); //实体主体的位置范围
    Entity_HEAD.insert(std::make_pair("Content-Type", "")); //实体主体的媒体类型
    Entity_HEAD.insert(std::make_pair("Expires", "")); //实体主体过期的日期时间
    Entity_HEAD.insert(std::make_pair("Last-Modified", "")); //资源的最后修改日期时间
}*/

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
            std::string response;
            std::string suffix;
            //HTTP_Response_Map.push_back(std::move(
            //      std::make_pair(std::move(std::string("Server")), std::move(std::string("plusplusi/0.5 Linux")))));

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
                //HTTP_Response_Map.push_back(std::move(std::make_pair(std::string("Content-Type"), HTTP_MIME_MAP[suffix])));
                HTTP_Response_Map.insert(std::move(std::make_pair(std::string("Content-Type"), HTTP_MIME_MAP[suffix])));
            } else
            {
                //else, handle it as plain text
                //HTTP_Response_Map.push_back(std::move(
                //      std::make_pair(std::string("Content-Type"), std::move(std::string("text/plain")))));
                HTTP_Response_Map.insert(std::move(
                        std::make_pair(std::string("Content-Type"), std::move(std::string("text/plain")))));
                suffix = "txt";
            }
            //std::cout << "file path is: " << path << std::endl;
            response = std::move(read_file(std::move(path)));
            //HTTP_Response_Map.push_back(std::make_pair(std::move(std::string("Content-Length")),
            //                                std::move(std::to_string(response.length()))));
            HTTP_Response_Map.insert(std::make_pair(std::move(std::string("Content-Length")),
                                                    std::move(std::to_string(response.length()))));
            std::string res_head =
                    request_line.at(2) + " " + std::to_string(status) + " " + HTTP_STATUS_CODE_MAP[status] + "\r\n";
            std::string field = std::move(map_to_string(HTTP_Response_Map));
            int ret = send_to_client(res_head);
            ret = send_to_client(field);
            ret = send_to_client(response);
            if (ret < 0)
            {
                std::cout << "write info to connection socket failed" << std::endl;
            }
            close_sock(CLIENT_SOCK);
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
        is.seekg(0, is.end);
        auto file_length = is.tellg();
        is.seekg(0, is.beg);
        char *buffer = new char[file_length];
        is.read(buffer, file_length);
        std::string image(buffer, file_length);
        status = 200;
        delete[] buffer;
        return image;
    }
    /*std::ostringstream buf;
    char ch;

    std::ifstream infile(filename);
    if (!infile)
    {
        status = 404;
        return buf.str();
    } else
    {
        while (buf && infile.get(ch)) buf.put(ch);
        status = 200;
        return buf.str();
    }*/
}

int HTTP_Handler::send_to_client(std::string &message)
{
    //send(CLIENT_SOCK, message.data(), message.size(), 0);
    auto body_length = message.size();
    char *buffer = new char[body_length];
    memcpy(buffer, message.data(), body_length);
    int ret = write(CLIENT_SOCK, buffer, body_length);
    delete[] buffer;
    return ret;
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
