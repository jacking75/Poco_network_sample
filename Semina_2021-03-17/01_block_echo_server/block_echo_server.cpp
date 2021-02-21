#define POCO_STATIC

#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketAddress.h"


const Poco::UInt16 PORT = 11021;

int main()
{
	std::cout << "서버 초기화 시작" << std::endl;

	Poco::Net::SocketAddress server_add(PORT);
	Poco::Net::ServerSocket server_sock(server_add);
	
	std::cout << "서버 초기화 완료. 클라이언트 접속 대기 중..." << std::endl;


	while (true)
	{
		Poco::Net::StreamSocket ss = server_sock.acceptConnection();

		try
		{							
			while (true)
			{
				char buffer[256] = { 0, };
				int n = ss.receiveBytes(buffer, sizeof(buffer));
				std::cout << "클라이언트에서 받은 메시지: " << buffer << std::endl;

				if (n <= 0) {
					break;
				}


				char szSendMessage[256] = { 0, };
				sprintf_s(szSendMessage, 256 - 1, "Re:%s", buffer);
				int nMsgLen = (int)strnlen_s(szSendMessage, 256 - 1);

				ss.sendBytes(szSendMessage, nMsgLen);				
			}

			std::cout << "클라이언트와 연결이 끊어졌습니다" << std::endl;				
		}
		catch (Poco::Exception& exc)
		{
			std::cout << "EchoServer: " << exc.displayText() << std::endl;
		}
	}

	getchar();
	return 0;
}