#include <string>
#include <iostream>

#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/HTTPClientSession.h>

#include <Poco/Net/WebSocket.h>
#include <Poco/Net/NetException.h>




int main()
{	
	int flags;
		
	try
	{
		Poco::Net::HTTPClientSession cs("localhost", 19980);
		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, "/ws", "HTTP/1.1");
		Poco::Net::HTTPResponse response;
		std::string cmd;

		Poco::Net::WebSocket * ws = new Poco::Net::WebSocket(cs, request, response); 

		std::string payload = "SGClient: Hello World!";
		ws->sendFrame(payload.data(), payload.size(), Poco::Net::WebSocket::FRAME_TEXT);

		char buffer[1024] = { 0, };
		int recvDataSize = ws->receiveFrame(buffer, sizeof(buffer), flags);
		std::cout << "[recv]" << buffer << std::endl;

		while (true)
		{
			cmd = "";
			std::cin >> cmd;

			if (cmd == "exit")
			{
				break;
			}

			ws->sendFrame(cmd.data(), cmd.size(), Poco::Net::WebSocket::FRAME_TEXT);
		
			char buffer[1024] = { 0, };
			int recvDataSize = ws->receiveFrame(buffer, sizeof(buffer), flags);
			if (recvDataSize > 0)
			{
				std::cout << "[recv]" << buffer << std::endl;
			}
		}

		ws->shutdown();
	}
	catch (Poco::Exception ex)
	{
		std::cout << ex.displayText() << std::endl;
		return 0;
	}
	 
	return 0;
}



