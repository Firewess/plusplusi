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
#include "plusplusi_function_objects.h"

int parse_http(const std::string& http)
{
    HTTP_REQ parse;
    parse(http);
}

#endif //_PLUSPLUSI_PARSEHTTP_H_
