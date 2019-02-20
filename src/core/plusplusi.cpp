#include <iostream>
#include <unistd.h>
#include <cstdio>
#include <cctype>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>

#include "plusplusi_config.h"
#include "../http/plusplusi_server.h"

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