#pragma once

#define POCO_STATIC


#pragma comment(lib, "ws2_32")
#pragma comment(lib, "IPHLPAPI.lib")  


#include <Poco/Net/SocketReactor.h>
#include <Poco/Net/ParallelSocketAcceptor.h>

#include "TcpSession.h"

namespace NetLib
{
	class ServerNet
	{
	public:
		ServerNet() {}
		virtual ~ServerNet() {}

		void Start(int port)
		{
			m_ServerSocket.bind(port, true);
			m_pAcceptor = new Poco::Net::ParallelSocketAcceptor<TcpSession, Poco::Net::SocketReactor>(m_ServerSocket, m_Reactor);

			m_Reactor.run();
		}


	private:
		Poco::Net::ServerSocket m_ServerSocket;
		Poco::Net::SocketReactor m_Reactor;
		Poco::Net::ParallelSocketAcceptor<TcpSession, Poco::Net::SocketReactor>* m_pAcceptor;
	};
}