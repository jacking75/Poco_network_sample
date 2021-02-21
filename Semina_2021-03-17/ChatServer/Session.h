#pragma once

#include <iostream>
#include <functional>

class Session 
{
public:
	Session(Poco::Net::StreamSocket& socket, Poco::Net::SocketReactor& reactor) :
		m_socket(socket),
		m_reactor(reactor)
	{
		m_peerAddress = socket.peerAddress().toString();
		std::cout << "connection from " << m_peerAddress << " ..." << std::endl;

		m_reactor.addEventHandler(m_socket,
			Poco::Observer<Session, Poco::Net::ReadableNotification>(*this, &Session::onReadable));

		OnConnection(this);
	}

	~Session()
	{
		std::cout << m_peerAddress << " disconnected ..." << std::endl;

		m_reactor.removeEventHandler(m_socket,
			Poco::Observer<Session, Poco::Net::ReadableNotification>(*this, &Session::onReadable)
		);
	}


	static std::function<void(Session*)> OnConnection;
	static std::function<void(Session*)> OnClose;
	static std::function<void(const int, const short, const short, char*)> AddPacketFunc;


	void SetIndex(const int index) { mIndex = index; }
	int GetIndex() { return mIndex; }


	void onReadable(Poco::Net::ReadableNotification* pNf)
	{
		pNf->release();

		try
		{
			char buffer[256] = { 0, };
			int n = m_socket.receiveBytes(buffer, sizeof(buffer));
			if (n > 0)
			{
				std::cout << "클라이언트에서 받은 메시지: " << buffer << std::endl;
				AddPacketFunc(10, 10, 10, nullptr);
				//char szSendMessage[256] = { 0, };
				//sprintf_s(szSendMessage, 256 - 1, "Re:%s", buffer);
				//int nMsgLen = (int)strnlen_s(szSendMessage, 256 - 1);
				//m_socket.sendBytes(szSendMessage, nMsgLen);
			}
			else
			{
				m_socket.close();
				//delete this; 
				OnClose(this);				
			}
		}
		catch (Poco::Exception ex)
		{
			m_socket.close();
			//delete this; 
			OnClose(this);
		}		
	}




private:
	Poco::Net::StreamSocket   m_socket;
	std::string m_peerAddress;
	Poco::Net::SocketReactor& m_reactor;

	int mIndex;
};

inline std::function<void(Session*)> Session::OnConnection;
inline std::function<void(Session*)> Session::OnClose;
inline std::function<void(const int, const short, const short, char*)> Session::AddPacketFunc;
