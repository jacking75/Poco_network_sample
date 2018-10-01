#pragma once

#include <iostream>

#include "../ReactorNetLib/ServerNet.h"

class EchoServer
{
public:
	EchoServer() {}
	~EchoServer() {}

	void Start(int port)
	{
		serverNet.Start(port);
	}

private:
	NetLib::ServerNet serverNet;
};