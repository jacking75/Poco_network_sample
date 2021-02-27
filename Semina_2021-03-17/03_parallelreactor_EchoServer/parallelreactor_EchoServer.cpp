#include <iostream>

#pragma comment(lib, "ws2_32")
#pragma comment(lib, "IPHLPAPI.lib")  

#define POCO_STATIC
#include <Poco/Net/SocketReactor.h>
#include <Poco/Net/ParallelSocketAcceptor.h>

const Poco::UInt16 PORT = 11021;

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

		//Poco::Thread::sleep(5000); //�ּ��� Ǯ�� 5�ʰ� �ٸ� Ŭ���̾�Ʈ ���� ó���� ���
	}

	~Session()
	{
		std::cout << m_peerAddress << " disconnected ..." << std::endl;

		m_reactor.removeEventHandler(m_socket,
			Poco::Observer<Session, Poco::Net::ReadableNotification>(*this, &Session::onReadable)
			);		
	}

	void onReadable(Poco::Net::ReadableNotification* pNf)
	{
		pNf->release();

		try
		{
			char buffer[256] = { 0, };
			int n = m_socket.receiveBytes(buffer, sizeof(buffer));
			if (n > 0)
			{
				std::cout << "Ŭ���̾�Ʈ���� ���� �޽���: " << buffer << std::endl;

				char szSendMessage[256] = { 0, };
				sprintf_s(szSendMessage, 256 - 1, "Re:%s", buffer);
				int nMsgLen = (int)strnlen_s(szSendMessage, 256 - 1);

				m_socket.sendBytes(szSendMessage, nMsgLen);

				//Poco::Thread::sleep(5000); // �����庰�� Ŭ���̾�Ʈ �Ҵ�ǰ�, ���� �ٸ� �����忡 ���ӵ� Ŭ���̾�Ʈ�� �������� �ʴ´�
				std::cout << "onReadable ��� ��" << buffer << std::endl;
			}
			else
			{
				m_socket.close();
				delete this;  // �޸� �������� ������ �Ҹ��ڰ� ȣ����� �ʴ´�.
			}
		}
		catch (Poco::Exception ex)
		{
			m_socket.close();
			delete this;  
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