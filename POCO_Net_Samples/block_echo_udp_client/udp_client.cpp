#include <list>
#include <tuple>
#include <sstream>

#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/MulticastSocket.h>


const Poco::UInt16 MY_PORT = 32451;
const Poco::UInt16 OTHER_PORT = 32452;


int main()
{
	Poco::Net::DatagramSocket sock(MY_PORT);
	//Poco::Net::SocketAddress client;

	try
	{
		for (int i = 0; i < 7; ++i)
		{
			char szMessage[256] = { 0, };
			sprintf_s(szMessage, 256 - 1, "%d - Send Message", i);
			int nMsgLen = (int)strnlen_s(szMessage, 256 - 1);

			sock.sendTo(szMessage, nMsgLen, Poco::Net::SocketAddress("localhost", OTHER_PORT));

			std::cout << "서버에 보낸 메시지: " << szMessage << std::endl;


			char buffer[256] = { 0, };
			Poco::Net::SocketAddress sender;
			auto len = sock.receiveFrom(buffer, sizeof(buffer), sender);

			if (len > 0)
			{
				std::cout << "서버로부터 받은 메시지: " << buffer << std::endl;
			}			
		}
	}
	catch (Poco::Exception& exc)
	{
		std::cout << "서버 접속 실패: " << exc.displayText() << std::endl;
	}
	//while (true)
	//{
	//	int bytes_read = 0;
	//	char buffer[254] = {};

	//	//BLOCKING FUNCTION !
	//	bytes_read = the_sock.receiveFrom(buffer, 254, client);

	//	if (bytes_read != 0)
	//	{
	//		std::cout << buffer << " from " << client.host().toString() << ":" << client.port() << std::endl;
	//		the_sock.sendTo(buffer, bytes_read, client, 0);
	//	}
	//}

	getchar();
	return 0;
}