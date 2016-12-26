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
		
		// 데이터 읽기 가능할 때 발생하는 이벤트
		m_Reactor.addEventHandler(m_Socket,
			Poco::Observer<Session, Poco::Net::ReadableNotification>(*this, &Session::onReadable));

		// 클라이언트가 접속을 끊었을 때 발생하는 이벤트
		m_Reactor.addEventHandler(m_Socket, 
			Poco::Observer<Session, Poco::Net::ShutdownNotification>(*this, &Session::onShutdown)); 
		
		// 소켓에 에러가 생겼을 때 발생하는 이벤트
		m_Reactor.addEventHandler(m_Socket, 
			Poco::Observer<Session, Poco::Net::ErrorNotification>(*this, &Session::onError)); 
		
		// Poco::Net::select 호출 시 발생하는 이벤트
		m_Reactor.addEventHandler(m_Socket, 
			Poco::Observer<Session, Poco::Net::IdleNotification>(*this, &Session::onIdle)); 
		
		// Poco::Net::select 호출 후 지정 시간까지 이벤트가 없을 때 발생하는 이벤트
		m_Reactor.addEventHandler(m_Socket, 
			Poco::Observer<Session, Poco::Net::TimeoutNotification>(*this, &Session::onTimeout)); 

	}

	~Session()
	{
		std::cout << m_PeerAddress << " disconnected ..." << std::endl;
		m_Reactor.removeEventHandler(m_Socket,
			Poco::Observer<Session, Poco::Net::ReadableNotification>(*this, &Session::onReadable)
			);

		m_Reactor.removeEventHandler(m_Socket, Poco::Observer<Session, Poco::Net::ShutdownNotification>(*this, &Session::onShutdown));
		m_Reactor.removeEventHandler(m_Socket, Poco::Observer<Session, Poco::Net::ErrorNotification>(*this, &Session::onError));
		m_Reactor.removeEventHandler(m_Socket, Poco::Observer<Session, Poco::Net::IdleNotification>(*this, &Session::onIdle));
		m_Reactor.removeEventHandler(m_Socket, Poco::Observer<Session, Poco::Net::TimeoutNotification>(*this, &Session::onTimeout));
	}

	void onReadable(Poco::Net::ReadableNotification* pNf)
	{
		pNf->release();

		char buffer[256] = { 0, };
		int n = m_Socket.receiveBytes(buffer, sizeof(buffer));
		if (n > 0)
		{
			std::cout << "클라이언트에서 받은 메시지: " << buffer << std::endl;

			char szSendMessage[256] = { 0, };
			sprintf_s(szSendMessage, 256 - 1, "Re:%s", buffer);
			int nMsgLen = (int)strnlen_s(szSendMessage, 256 - 1);

			m_Socket.sendBytes(szSendMessage, nMsgLen);
		}
		else
		{
			m_Socket.shutdownSend();
			delete this;
		}
	}

	void onShutdown(Poco::Net::ShutdownNotification* pNf)
	{
		pNf->release();
	}

	void onError(Poco::Net::ErrorNotification* pNf)
	{
		pNf->release();

		std::cout << "onError 발생" << std::endl;
	}

	void onTimeout(Poco::Net::TimeoutNotification* pNf)
	{
		pNf->release();

		std::cout << "onTimeout 발생" << std::endl;
	}

	void onIdle(Poco::Net::IdleNotification* pNf)
	{
		pNf->release();

		std::cout << "onIdle 발생" << std::endl;
	}

private:
	Poco::Net::StreamSocket   m_Socket;
	std::string m_PeerAddress;
	Poco::Net::SocketReactor& m_Reactor;
};



int main()
{
	Poco::Timespan timeout(10, 0);
	Poco::Net::SocketReactor reactor(timeout);

	Poco::Net::ServerSocket serverSocket(PORT);

	Poco::Net::SocketAcceptor<Session> acceptor(serverSocket, reactor);


	std::cout << "Reactor: starting..." << std::endl;

	reactor.run();


	std::cout << "Reactor: 종료..." << std::endl;
	getchar();
	return 0;
}