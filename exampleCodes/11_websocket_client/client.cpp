#include <string>
#include <iostream>

#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/HTTPClientSession.h>

#include <Poco/Net/WebSocket.h>
#include <Poco/Net/NetException.h>




int main()
{
	char buffer[1024];
	int flags;
	int n;
	std::string payload;

	try
	{
		Poco::Net::HTTPClientSession cs("localhost", 8080);
		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, "/ws", "HTTP/1.1");
		Poco::Net::HTTPResponse response;
		std::string cmd;

		Poco::Net::WebSocket * ws = new Poco::Net::WebSocket(cs, request, response); 

		payload = "SGClient: Hello World!";
		ws->sendFrame(payload.data(), payload.size(), Poco::Net::WebSocket::FRAME_TEXT);
		n = ws->receiveFrame(buffer, sizeof(buffer), flags);

		while (cmd != "exit")
		{
			cmd = "";
			std::cin >> cmd;
			ws->sendFrame(cmd.data(), cmd.size(), Poco::Net::WebSocket::FRAME_TEXT);
			
			n = ws->receiveFrame(buffer, sizeof(buffer), flags);
			if (n > 0)
			{
				std::cout << buffer << std::endl;
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



