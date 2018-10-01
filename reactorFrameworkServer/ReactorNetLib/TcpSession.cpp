#include "TcpSession.h"

namespace NetLib
{
	TcpSession::TcpSession(Poco::Net::StreamSocket& socket, Poco::Net::SocketReactor& reactor) :
		m_Socket(socket),
		m_Reactor(reactor)
	{
		m_PeerAddress = socket.peerAddress().toString();
		std::cout << "connection from " << m_PeerAddress << " ..." << std::endl;

		m_Reactor.addEventHandler(m_Socket,
			Poco::Observer<TcpSession, Poco::Net::ReadableNotification>(*this, &TcpSession::onReadable));
	}

	TcpSession::~TcpSession()
	{
		std::cout << m_PeerAddress << " disconnected ..." << std::endl;

		m_Reactor.removeEventHandler(m_Socket,
			Poco::Observer<TcpSession, Poco::Net::ReadableNotification>(*this, &TcpSession::onReadable)
		);
	}

	void TcpSession::onReadable(Poco::Net::ReadableNotification* pNf)
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

			//Poco::Thread::sleep(10000); // 스레드별로 클라이언트 할당되고, 서로 다른 스레드에 접속된 클라이언트는 간섭하지 않는다
			std::cout << "onReadable 대기 끝" << buffer << std::endl;
		}
		else
		{
			m_Socket.shutdownSend();
			delete this;  // 메모리 해제하지 않으면 소멸자가 호출되지 않는다.
		}
	}
}