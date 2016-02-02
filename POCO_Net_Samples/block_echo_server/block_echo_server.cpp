
#include "Poco/Net/Net.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Timespan.h"

const Poco::UInt16 PORT = 32452;

int main()
{
	std::cout << "서버 초기화 시작" << std::endl;

	Poco::Net::SocketAddress server_add(PORT);
	Poco::Net::ServerSocket server_sock(server_add);
	
	std::cout << "서버 초기화 완료. 클라이언트 접속 대기 중..." << std::endl;


	Poco::Timespan span(250000);
	while (true)
	{
		Poco::Net::StreamSocket ss = server_sock.acceptConnection();
		try
		{
			char buffer[256] = { 0, };
			int n = ss.receiveBytes(buffer, sizeof(buffer));
			std::cout << "클라이언트에서 받은 메시지: " << buffer << std::endl;
				
			while (n > 0)
			{
				char szSendMessage[256] = { 0, };
				sprintf_s(szSendMessage, 128 - 1, "Re:%s", buffer);
				int nMsgLen = strnlen_s(szSendMessage, 256 - 1);

				ss.sendBytes(szSendMessage, nMsgLen);


				n = ss.receiveBytes(buffer, sizeof(buffer));
				std::cout << "클라이언트에서 받은 메시지: " << buffer << std::endl;
			}

			std::cout << "클라이언트와 연결이 끊어졌습니다" << std::endl;				
		}
		catch (Poco::Exception& exc)
		{
			std::cerr << "EchoServer: " << exc.displayText() << std::endl;
		}
	}

	getchar();
	return 0;
}