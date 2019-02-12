/*
 *@author: Jie Feng
 *@desription: todo
 *@date: 19-2-11 下午11:29
 */

#ifndef _PLUSPLUSI_PARSEHTTP_H_
#define _PLUSPLUSI_PARSEHTTP_H_

#include <string>
#include <cstring>
#include <vector>
#include <map>

/*
 *@author: Jie Feng
 *@desription: 辅助函数，将字符串按照分隔符进行分割
 *@date: 19-2-11 下午11:29
 */
std::vector<std::string> split_string(const std::string& str, const std::string& separator)
{
    std::vector<std::string> sub_strings;
    std::string::size_type posSubstringStart; //子串开始位置

    std::string::size_type posSeparator;        //分隔符位置

    posSeparator= str.find(separator);
    posSubstringStart= 0;
    while (std::string::npos != posSeparator)
    {
        sub_strings.push_back(str.substr(posSubstringStart, posSeparator- posSubstringStart));

        posSubstringStart= posSeparator+ separator.size();
        posSeparator= str.find(separator, posSubstringStart);
    }

    if (posSubstringStart!= str.length())  // 截取最后一段数据
        sub_strings.push_back(str.substr(posSubstringStart));

    return sub_strings;
}

int parse_http(const std::string & http)
{
    auto strs = split_string(http, "\r\n");

    if(strs.empty()) return -1;
    else
    {

    }
}

#endif //_PLUSPLUSI_PARSEHTTP_H_
