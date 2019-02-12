cmake_minimum_required(VERSION 3.13)
project(plusplisi)

set(CMAKE_CXX_STANDARD 14)

add_executable(plusplusi src/core/plusplusi.cpp src/core/plusplusi_config.h src/core/plusplusi_config.cpp src/http/plusplusi_server.h src/http/plusplusi_parsehttp.h src/http/plusplusi_function_objects.h)