#include <iostream>
#include "plusplusi_config.h"
#include <unistd.h>
#include "../http/plusplusi_server.h"

int main()
{
    run();

    int port;
	const std::string ConfigFile = "../conf/configure.txt";
	Config configSettings(ConfigFile);

	port = configSettings.Read("port", 0);
	std::cout << "port:" << port << std::endl;

	return 0;
}