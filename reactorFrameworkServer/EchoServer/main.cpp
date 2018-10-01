#include <iostream>

#include "EchoServer.h"


const int PORT = 32452;

int main()
{
	std::cout << "Echo Server Start...." << std::endl;

	EchoServer server;
	server.Start(PORT);
	
	return 0;
}