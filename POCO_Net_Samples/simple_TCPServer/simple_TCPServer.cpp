// 출처: http://codeit.blog.fc2.com/blog-entry-6.html
// 간단 TCP 서버

#include <iostream>

#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/TCPServer.h>
#include <Poco/Net/TCPServerConnection.h>
#include <Poco/Net/TCPServerConnectionFactory.h>

const Poco::UInt16 SERVER_PORT = 8888;

class MyConnection : public Poco::Net::TCPServerConnection
{
public:
	MyConnection(const Poco::Net::StreamSocket &socket) : TCPServerConnection(socket) {    }
	virtual ~MyConnection() {    }

	virtual void run()
	{
		static const char message[] = "This is TCP server sample\n";
		socket().sendBytes(message, sizeof(message) - 1);
	}
};

class MyConnectionFactory : public Poco::Net::TCPServerConnectionFactory
{
public:
	MyConnectionFactory() {    }
	virtual ~MyConnectionFactory() {    }

	virtual Poco::Net::TCPServerConnection* createConnection(const Poco::Net::StreamSocket &socket)
	{
		return new MyConnection(socket);
	}
};

int main(int, char**)
{
	Poco::Net::ServerSocket sock(SERVER_PORT);
	sock.listen();

	Poco::Net::TCPServer server(new MyConnectionFactory(), sock);

	std::cout << "Simple TCP Server Application." << std::endl;
	std::cout << "command: q | quit : Quit application." << std::endl;

	server.start();

	bool running = true;
	while (running)
	{
		char buff[256];
		if (fgets(buff, sizeof(buff), stdin) == NULL)
		{
			running = false;
		}
		else
		{
			buff[strlen(buff) - 1] = '\0';

			if (strcmp(buff, "quit") == 0 || strcmp(buff, "q") == 0)
			{
				printf(">> QUIT command accepted.\n");
				running = false;
			}
			else
			{
				printf(">> UNKNOWN command.\n");
			}
		}
	}

	server.stop();
	return 0;
}