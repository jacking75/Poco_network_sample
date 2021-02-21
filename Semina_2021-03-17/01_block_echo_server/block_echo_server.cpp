#define POCO_STATIC

#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketAddress.h"


const Poco::UInt16 PORT = 11021;

int main()
{
	std::cout << "���� �ʱ�ȭ ����" << std::endl;

	Poco::Net::SocketAddress server_add(PORT);
	Poco::Net::ServerSocket server_sock(server_add);
	
	std::cout << "���� �ʱ�ȭ �Ϸ�. Ŭ���̾�Ʈ ���� ��� ��..." << std::endl;


	while (true)
	{
		Poco::Net::StreamSocket ss = server_sock.acceptConnection();

		try
		{							
			while (true)
			{
				char buffer[256] = { 0, };
				int n = ss.receiveBytes(buffer, sizeof(buffer));
				std::cout << "Ŭ���̾�Ʈ���� ���� �޽���: " << buffer << std::endl;

				if (n <= 0) {
					break;
				}


				char szSendMessage[256] = { 0, };
				sprintf_s(szSendMessage, 256 - 1, "Re:%s", buffer);
				int nMsgLen = (int)strnlen_s(szSendMessage, 256 - 1);

				ss.sendBytes(szSendMessage, nMsgLen);				
			}

			std::cout << "Ŭ���̾�Ʈ�� ������ ���������ϴ�" << std::endl;				
		}
		catch (Poco::Exception& exc)
		{
			std::cout << "EchoServer: " << exc.displayText() << std::endl;
		}
	}

	getchar();
	return 0;
}