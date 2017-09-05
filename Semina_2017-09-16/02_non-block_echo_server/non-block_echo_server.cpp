#define POCO_STATIC

#include <algorithm>

#include "Poco/Net/Net.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/SocketAddress.h"

const Poco::UInt16 PORT = 32452;

int main()
{
	std::cout << "서버 초기화 시작" << std::endl;

	Poco::Net::SocketAddress server_add(PORT);
	Poco::Net::ServerSocket server_sock(server_add);

	std::cout << "서버 초기화 완료. 클라이언트 접속 대기 중..." << std::endl;

	Poco::Net::Socket::SocketList connectedSockList;
	connectedSockList.push_back(server_sock);

	while (true)
	{
		Poco::Net::Socket::SocketList readList(connectedSockList.begin(), connectedSockList.end());
		Poco::Net::Socket::SocketList writeList(connectedSockList.begin(), connectedSockList.end());
		Poco::Net::Socket::SocketList exceptList(connectedSockList.begin(), connectedSockList.end());
		
		Poco::Timespan timeout(1);

		auto count = Poco::Net::Socket::select(readList, writeList, exceptList, timeout);

		if (count == 0) {
			continue;
		}

		Poco::Net::Socket::SocketList delSockList;

		for (auto& readSock : readList)
		{
			if (server_sock == readSock)
			{
				auto newSock = server_sock.acceptConnection();
				connectedSockList.push_back(newSock);

				std::cout << "새 클라이언트에서 접속" << std::endl;
			}
			else
			{
				char buffer[256] = { 0, };
				auto n = ((Poco::Net::StreamSocket*)&readSock)->receiveBytes(buffer, sizeof(buffer));

				if (n > 0) 
				{
					std::cout << "클라이언트에서 받은 메시지: " << buffer << std::endl;


					char szSendMessage[256] = { 0, };
					sprintf_s(szSendMessage, 256 - 1, "Re:%s", buffer);
					int nMsgLen = (int)strnlen_s(szSendMessage, 256 - 1);

					((Poco::Net::StreamSocket*)&readSock)->sendBytes(szSendMessage, nMsgLen);
				}
				else 
				{
					std::cout << "클라이언트와 연결이 끊어졌습니다" << std::endl;

					delSockList.push_back(readSock);
					
				}
			}
		}

		for (auto& delSock : delSockList)
		{
			auto delIter = std::find_if(connectedSockList.begin(), connectedSockList.end(), 
										[&delSock](auto& sock) { return delSock == sock ? true : false;}
									);

			if (delIter != connectedSockList.end()) {
				connectedSockList.erase(delIter);
				
				std::cout << "connectedSockList 에서 socket 제거" << std::endl;
			}
		}		
	}

	getchar();
	return 0;
}