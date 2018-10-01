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
			std::cout << "Ŭ���̾�Ʈ���� ���� �޽���: " << buffer << std::endl;

			char szSendMessage[256] = { 0, };
			sprintf_s(szSendMessage, 256 - 1, "Re:%s", buffer);
			int nMsgLen = (int)strnlen_s(szSendMessage, 256 - 1);

			m_Socket.sendBytes(szSendMessage, nMsgLen);

			//Poco::Thread::sleep(10000); // �����庰�� Ŭ���̾�Ʈ �Ҵ�ǰ�, ���� �ٸ� �����忡 ���ӵ� Ŭ���̾�Ʈ�� �������� �ʴ´�
			std::cout << "onReadable ��� ��" << buffer << std::endl;
		}
		else
		{
			m_Socket.shutdownSend();
			delete this;  // �޸� �������� ������ �Ҹ��ڰ� ȣ����� �ʴ´�.
		}
	}
}