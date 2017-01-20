#include <iostream>

#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

#include <Poco/Net/WebSocket.h>
#include <Poco/Net/NetException.h>

class WebSocketRequestHandler : public Poco::Net::HTTPRequestHandler
{
public:
	void handleRequest(Poco::Net::HTTPServerRequest& request, 
						Poco::Net::HTTPServerResponse& response)
	{
		try
		{
			Poco::Net::WebSocket ws(request, response);
			
			std::cout << "WebSocket connection established" << std::endl;
			
			int flags = 0;
			int recvDataSize = 0;

			while(true)
			{
				char buffer[1024] = { 0, };
				
				recvDataSize = ws.receiveFrame(buffer, sizeof(buffer), flags);
				
				if (recvDataSize > 0)
				{
					char log[1024] = { 0, };
					sprintf_s(log, 1024, "Frame received (length=%d, flags=0x%x): %s", recvDataSize, unsigned(flags), buffer);
					std::cout << log << std::endl;

					ws.sendFrame(buffer, recvDataSize, flags);
				}

				if (recvDataSize <= 0 ||
					(flags & Poco::Net::WebSocket::FRAME_OP_BITMASK) != Poco::Net::WebSocket::FRAME_OP_CLOSE)
				{
					break;
				}
			};
			
			std::cout << "WebSocket connection closed." << std::endl;
		}
		catch (Poco::Net::WebSocketException& exc)
		{
			std::cout << exc.displayText() << std::endl;

			switch (exc.code())
			{
			case Poco::Net::WebSocket::WS_ERR_HANDSHAKE_UNSUPPORTED_VERSION:
				response.set("Sec-WebSocket-Version", Poco::Net::WebSocket::WEBSOCKET_VERSION);
				// fallthrough
			case Poco::Net::WebSocket::WS_ERR_NO_HANDSHAKE:
			case Poco::Net::WebSocket::WS_ERR_HANDSHAKE_NO_VERSION:
			case Poco::Net::WebSocket::WS_ERR_HANDSHAKE_NO_KEY:
				response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
				response.setContentLength(0);
				response.send();
			}
		}
	}
};

class DummyRequestHandler : public Poco::Net::HTTPRequestHandler
{
public:

	void handleRequest(Poco::Net::HTTPServerRequest& request,
		Poco::Net::HTTPServerResponse& response)
	{
		std::cout << "DummyRequestHandler: " << request.getURI() << std::endl;

		response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_OK);
		const char * data = "Ok.\n";
		response.sendBuffer(data, strlen(data));
	}
};

class SimpleRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
{
public:

	Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request)
	{
		std::cout << "SimpleRequestHandlerFactory: " << request.getURI() << std::endl;

		if (request.find("Upgrade") != request.end() && Poco::icompare(request["Upgrade"], "websocket") == 0) {
			return new WebSocketRequestHandler;
		}
				
		return new DummyRequestHandler;
	}
};

int main()
{
	// 서버의 port는 19980 
	Poco::Net::ServerSocket svs(19980);

	Poco::Net::HTTPServerParams* pParams = new Poco::Net::HTTPServerParams;
	Poco::Timespan timeout(300, 0); // 300초
	pParams->setTimeout(timeout);
	
		// HTTP 서버 클래스 인스턴스
	Poco::Net::HTTPServer srv(new SimpleRequestHandlerFactory(), svs, pParams);

	
	// 서버 시작
	srv.start();

	std::cout << "Port 19980으로 서버 시작" << std::endl;

	getchar();

	// 서버 동작 중단
	srv.stop();

	return 0;
}