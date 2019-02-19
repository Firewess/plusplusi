/*
 *@author: Jie Feng
 *@desription: this file is only used for function test
 *@date: 19-2-13 下午5:00
 */

#include "../src/core/plusplusi_config.h"
#include "../src/http/plusplusi_server.h"
#include <algorithm>
#include <vector>

int main()
{
    int PORT = 1114;
    int WORKER = 4;
    std::string ROOT = "../html";
    std::string INDEX = "index.html";
    const std::string ConfigFile = "../conf/plusplusi.conf";
    Config settings(ConfigFile);

    PORT = settings.Read("port", 1114);
    WORKER = settings.Read("worker", 1);
    ROOT = settings.Read<std::string>("root", "../html");
    INDEX = settings.Read<std::string>("index", "index.html");

    std::cout << "port:" << PORT << std::endl;
    std::cout << "workers: " << WORKER << std::endl;

    HTTP_SERVER http_server(PORT, std::move(ROOT), std::move(INDEX), WORKER);
    http_server.run();

    return 0;
}