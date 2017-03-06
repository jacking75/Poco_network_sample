#include <iostream>

#include <Poco/Net/SocketReactor.h>
#include <Poco/Net/ParallelSocketAcceptor.h>

const Poco::UInt16 PORT = 32452;

class Session {
public:
	Session(Poco::Net::StreamSocket& socket, Poco::Net::SocketReactor& reactor) :
		m_socket(socket),
		m_reactor(reactor)
	{
		m_peerAddress = socket.peerAddress().toString();
		std::cout << "connection from " << m_peerAddress << " ..." << std::endl;

		m_reactor.addEventHandler(m_socket,
			Poco::Observer<Session, Poco::Net::ReadableNotification>(*this, &Session::onReadable));

		//Poco::Thread::sleep(10000); 주석을 풀면 10초간 다른 클라이언트 접속 처리를 대기
	}

	~Session()
	{
		std::cout << m_peerAddress << " disconnected ..." << std::endl;

		m_reactor.removeEventHandler(m_socket,
			Poco::Observer<Session, Poco::Net::ReadableNotification>(*this, &Session::onReadable)
			);

		Poco::Thread::sleep(10000);
	}

	void onReadable(Poco::Net::ReadableNotification* pNf)
	{
		pNf->release();

		char buffer[256] = { 0, };
		int n = m_socket.receiveBytes(buffer, sizeof(buffer));
		if (n > 0)
		{
			std::cout << "클라이언트에서 받은 메시지: " << buffer << std::endl;

			char szSendMessage[256] = { 0, };
			sprintf_s(szSendMessage, 256 - 1, "Re:%s", buffer);
			int nMsgLen = (int)strnlen_s(szSendMessage, 256 - 1);

			m_socket.sendBytes(szSendMessage, nMsgLen);

			Poco::Thread::sleep(10000); // 스레드별로 클라이언트 할당되고, 서로 다른 스레드에 접속된 클라이언트는 간섭하지 않는다
			std::cout << "onReadable 대기 끝" << buffer << std::endl;
		}
		else
		{
			m_socket.shutdownSend();
			delete this;  // 메모리 해제하지 않으면 소멸자가 호출되지 않는다.
		}
	}


private:
	Poco::Net::StreamSocket   m_socket;
	std::string m_peerAddress;
	Poco::Net::SocketReactor& m_reactor;
};


int main()
{
	Poco::Net::ServerSocket serverSocket(PORT);
	Poco::Net::SocketReactor reactor;
	Poco::Net::ParallelSocketAcceptor<Session, Poco::Net::SocketReactor> acceptor(serverSocket, reactor);

	std::cout << "Parallel Reactor: " << "starting..." << std::endl;
	reactor.run();

	return 0;
}