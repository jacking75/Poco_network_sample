#pragma once

#include <vector>

#include "TcpSession.h"

namespace NetLib
{
	class TcpClientSessionManager
	{
	public:
		TcpClientSessionManager() {}
		~TcpClientSessionManager() {}

		void Init(const int maxSessionCount)
		{

		}

	private:
		std::vector<TcpSession> m_SessionList;
	};
}