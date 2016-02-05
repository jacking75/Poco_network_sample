#include <iostream>

#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/MulticastSocket.h>


const Poco::UInt16 MY_PORT = 32452;
//const Poco::UInt16 OTHER_PORT = 32451;


int main()
{
	Poco::Net::DatagramSocket serv_sock(MY_PORT);
	Poco::Net::SocketAddress client;
			
	Poco::Timespan span(3, 0);
	while (true)
	{
		if (serv_sock.poll(span, Poco::Net::Socket::SELECT_READ))
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
	}


	//SocketAddress localhost(SERVER_IP, SERVER_PORT);
	//DatagramSocket the_sock(localhost);

	//SocketAddress client;

	//int bytes_read = 0;

	//cout << "server online at " << SERVER_IP << ":" << SERVER_PORT << " and listening !" << endl;

	//while (true)
	//{
	//	char buffer[254] = {};
	//	//BLOCKING FUNCTION !
	//	bytes_read = the_sock.receiveFrom(buffer, 254, client);
	//	if (bytes_read != 0)
	//	{
	//		cout << buffer << " from " << client.host().toString() << ":" << client.port() << endl;
	//		the_sock.sendTo(buffer, bytes_read, client, 0);
	//	}
	//}
		
	return(0);
}