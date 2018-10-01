#pragma once

#include <Poco/Net/SocketReactor.h>
#include <Poco/Net/ParallelSocketAcceptor.h>

namespace NetLib
{
	class TcpSession
	{
	public:
		TcpSession(Poco::Net::StreamSocket& socket, Poco::Net::SocketReactor& reactor);
		virtual ~TcpSession();

	private:
		void onReadable(Poco::Net::ReadableNotification* pNf);

	private:
		Poco::Net::StreamSocket   m_Socket;
		std::string m_PeerAddress;
		Poco::Net::SocketReactor& m_Reactor;
	};
}
