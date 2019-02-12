/*
 *@author: Jie Feng
 *@desription: this file implements some function objects used for parse http
 *@date: 19-2-12 上午10:32
 */

#ifndef _plusplusi_function_objects_H_
#define _plusplusi_function_objects_H_

#include <string>
#include <map>

#define GET 0
#define POST 1
#define HEAD 2
#define PUT 3
#define DELETE 4
#define CONNECT 5
#define OPTIONS 6
#define TRACE 7
#define PATCH 8

/*
 *@desription: 辅助函数，将字符串按照分隔符进行分割
 *@date: 19-2-11 下午11:29
 */
std::vector<std::string> split_string(const std::string& str, const std::string& separator)
{
    std::vector<std::string> sub_strings;
    std::string::size_type pos_substr_start;

    std::string::size_type pos_separator;

    pos_separator= str.find(separator);
    pos_substr_start= 0;
    while (std::string::npos != pos_separator)
    {
        sub_strings.push_back(str.substr(pos_substr_start, pos_separator- pos_substr_start));

        pos_substr_start= pos_separator+ separator.size();
        pos_separator= str.find(separator, pos_substr_start);
    }

    if (pos_substr_start!= str.length())
        sub_strings.push_back(str.substr(pos_substr_start));

    return sub_strings;
}

class HTTP_REQ
{
public:
    HTTP_REQ()
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
    }

    void operator() (const std::string& str)
    {
        auto strs = split_string(str, "\r\n");
        auto head = split_string(strs.at(0), " ");
        auto uri = split_string(head.at(1), "/");
        switch (http_method[head.at(0)])
        {
            case GET:

                break;
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

private:
    std::map<std::string, int> http_method;
};

class HTTP_RES
{
public:
    void operator() ()
    {

    }
};

#endif //_plusplusi_function_objects_H_
