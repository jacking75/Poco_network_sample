
#include "Poco/Net/Net.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Timespan.h"

const Poco::UInt16 PORT = 32452;

int main()
{
	Poco::Net::SocketAddress server_add(PORT);
	Poco::Net::ServerSocket server_sock(server_add);
	
	Poco::Timespan span(250000);
	while (true)
	{
		if (server_sock.poll(span, Poco::Net::Socket::SELECT_READ))
		{
			Poco::Net::StreamSocket ss = server_sock.acceptConnection();
			try
			{
				char buffer[256];
				int n = ss.receiveBytes(buffer, sizeof(buffer));
				
				while (n > 0)
				{
					ss.sendBytes(buffer, n);
					n = ss.receiveBytes(buffer, sizeof(buffer));
				}
			}
			catch (Poco::Exception& exc)
			{
				std::cerr << "EchoServer: " << exc.displayText() << std::endl;
			}
		}
	}

	return 0;
}