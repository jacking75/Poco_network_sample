#include <iostream>

#include <Poco/Net/TCPServer.h>
#include <Poco/Net/TCPServerConnection.h>
#include <Poco/Net/TCPServerConnectionFactory.h>
#include <Poco/Thread.h>

const Poco::UInt16 SERVER_PORT = 32452;

class Session : public Poco::Net::TCPServerConnection
{
public:
	Session(const Poco::Net::StreamSocket &socket) : TCPServerConnection(socket) {    
		std::cout << "Session 객체 생성" << std::endl;
	}

	virtual ~Session() {    
		std::cout << "Session 객체 소멸" << std::endl;
	}

	virtual void run()
	{
		try
		{
			int recvSize = 0;

			do
			{
				char buffer[256] = { 0, };
				recvSize = socket().receiveBytes(buffer, sizeof(buffer));
				std::cout << "클라이언트에서 받은 메시지: " << buffer << std::endl;


				char szSendMessage[256] = { 0, };
				sprintf_s(szSendMessage, 256 - 1, "Re:%s", buffer);
				int nMsgLen = (int)strnlen_s(szSendMessage, 256 - 1);

				socket().sendBytes(szSendMessage, nMsgLen);				
			} while (recvSize > 0);

			std::cout << "클라이언트와 연결이 끊어졌습니다" << std::endl;
		}
		catch (Poco::Exception& exc)
		{
			std::cout << "Session: " << exc.displayText() << std::endl;
		}
	}
};

class SessionFactory : public Poco::Net::TCPServerConnectionFactory
{
public:
	SessionFactory() { }
	virtual ~SessionFactory() { }

	virtual Poco::Net::TCPServerConnection* createConnection(const Poco::Net::StreamSocket &socket)
	{
		return new Session(socket);
	}
};

int main()
{
	Poco::Net::ServerSocket sock(SERVER_PORT);	
	Poco::Net::TCPServer server(new SessionFactory(), sock);

	std::cout << "Simple TCP Server Application." << std::endl;	
	std::cout << "maxThreads: " << server.maxThreads() << std::endl;	 
	std::cout << "maxConcurrentConnections: " << server.maxConcurrentConnections() << std::endl;
	
	
	server.start();
	
	std::cout << "start() 후 프로그램 종료가 되지 않도록 대기 하기" << std::endl;
	while (true)
	{		
		Poco::Thread::sleep(1);
	}

	return 0;
}