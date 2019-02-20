#cmake_minimum_required(VERSION 3.13)
project(plusplusi)

find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 14)
add_compile_options(-std=c++11)

add_executable(${PROJECT_NAME} src/core/plusplusi.cpp test/test1.cpp src/core/plusplusi_config.h src/core/plusplusi_config.cpp src/http/plusplusi_server.h src/http/plusplusi_handle_http.h src/http/plusplusi_epoll.h test/test.cpp src/http/plusplusi_handle_http.cpp src/http/plusplusi_server.cpp src/http/plusplusi_accept_mutex.h src/http/plusplusi_accept_mutex.cpp src/http/plusplusi_timer.h src/http/plusplusi_timer.cpp)

target_link_libraries(${PROJECT_NAME} Threads::Threads) #add multi-thread support