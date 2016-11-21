#include <iostream>

#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/DatagramSocket.h>


const Poco::UInt16 MY_PORT = 32452;


int main()
{
	Poco::Net::DatagramSocket serv_sock(MY_PORT);
	Poco::Net::SocketAddress client;
	
	while (true)
	{
		try
		{
			char buffer[256] = { 0, };
			Poco::Net::SocketAddress sender;
			int len = serv_sock.receiveFrom(buffer, sizeof(buffer), sender);

			if (len > 0)
			{
				std::cout << buffer << " from " << sender.host().toString() << ":" << sender.port() << std::endl;


				char szSendMessage[256] = { 0, };
				sprintf_s(szSendMessage, 256 - 1, "Re:%s", buffer);
				int nMsgLen = (int)strnlen_s(szSendMessage, 256 - 1);

				serv_sock.sendTo(szSendMessage, nMsgLen, sender);
			}
		}
		catch (Poco::Exception& exc)
		{
			std::cerr << "UDPEchoServer: " << exc.displayText() << std::endl;
		}
	}
		
	return(0);
}