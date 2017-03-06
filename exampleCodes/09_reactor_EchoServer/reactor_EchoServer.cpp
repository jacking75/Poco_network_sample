#include <iostream>

#include <Poco/Net/SocketReactor.h>
#include <Poco/Net/SocketAcceptor.h>

const Poco::UInt16 PORT = 32452;

class Session {
public:
	Session(Poco::Net::StreamSocket& socket, Poco::Net::SocketReactor& reactor) :
		m_Socket(socket),
		m_Reactor(reactor)
	{
		m_PeerAddress = socket.peerAddress().toString();
		std::cout << "connection from " << m_PeerAddress << " ..." << std::endl;
		
		m_Reactor.addEventHandler(m_Socket,
								Poco::Observer<Session, 
											Poco::Net::ReadableNotification>( 
												*this, &Session::onReadable)
								);

	}

	~Session()
	{
		std::cout << m_PeerAddress << " disconnected ..." << std::endl;
		m_Reactor.removeEventHandler(m_Socket,
									Poco::Observer<Session, 
											Poco::Net::ReadableNotification>(
												*this, &Session::onReadable)
									);
	}

	void onReadable(Poco::Net::ReadableNotification* pNotification)
	{
		pNotification->release();

		try
		{
			char buffer[256] = { 0, };
			int n = m_Socket.receiveBytes(buffer, sizeof(buffer));
			if (n > 0)
			{
				char szSendMessage[256] = { 0, };
				sprintf_s(szSendMessage, 256 - 1, "Re:%s", buffer);
				int nMsgLen = (int)strnlen_s(szSendMessage, 256 - 1);

				m_Socket.sendBytes(szSendMessage, nMsgLen);


				std::cout << "클라이언트에서 받은 메시지: " << buffer << std::endl;
			}
			else
			{
				m_Socket.shutdown();
				delete this;  // 메모리 해제하지 않으면 소멸자가 호출되지 않는다.
			}
		}
		catch (Poco::Exception& exc)
		{
			std::cout << "EchoServer: " << exc.displayText() << std::endl;

			m_Socket.shutdown();
			delete this;  
		}
	}


private:
	Poco::Net::StreamSocket   m_Socket;
	std::string m_PeerAddress;
	Poco::Net::SocketReactor& m_Reactor;
};


int main()
{
	Poco::Net::SocketReactor reactor;

	Poco::Net::ServerSocket serverSocket(PORT);
	
	Poco::Net::SocketAcceptor<Session> acceptor(serverSocket, reactor);

	
	std::cout << "Reactor: starting..." << std::endl;
	
	reactor.run();
	

	std::cout << "Reactor: 종료..." << std::endl;
	getchar();
	return 0;
}