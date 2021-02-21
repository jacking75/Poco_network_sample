#define POCO_STATIC

#include <iostream>

#include "ChatServer.h"

const Poco::UInt16 PORT = 11021;
const int MAX_CONNECT_COUNT = 256;

int main()
{
	ChatServer chatServer;

	chatServer.Init(MAX_CONNECT_COUNT);
	chatServer.Start(PORT);

	return 0;
}