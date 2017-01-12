#include <iostream>

#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/HTMLForm.h>



void PrintJsonData(Poco::Net::HTTPServerRequest& request)
{
	std::istream &i = request.stream();
	int len = request.getContentLength();
	char buffer[1024] = { 0, };
	i.read(buffer, len);
	std::cout << buffer << std::endl;
}

class GetRequestHandler : public Poco::Net::HTTPRequestHandler
{
public:

	void handleRequest(Poco::Net::HTTPServerRequest& request,
					Poco::Net::HTTPServerResponse& response)
	{
		std::cout << "GetRequestHandler: " << request.getURI() << std::endl;

		PrintJsonData(request);
		
		response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_OK);
		const char * data = "Ok.\n";
		response.sendBuffer(data, strlen(data));
	}
};

class PostRequestHandler : public Poco::Net::HTTPRequestHandler
{
public:

	void handleRequest(Poco::Net::HTTPServerRequest& request,
		Poco::Net::HTTPServerResponse& response)
	{
		std::cout << "PostRequestHandler: " << request.getURI() << std::endl;

		PrintJsonData(request);

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
		if (request.getMethod() == "POST") {
			return new PostRequestHandler;
		}
		
		return new GetRequestHandler;
	}
};

int main()
{	
	Poco::Net::HTTPServerParams* pParams = new Poco::Net::HTTPServerParams;
			
	Poco::Net::ServerSocket svs(19980);

	Poco::Net::HTTPServer    srv(new SimpleRequestHandlerFactory(), svs, pParams);

	
	// 서버 시작
	srv.start();

	std::cout << "Port 19980으로 서버 시작" << std::endl;

	getchar();

	// 서버 동작 중단
	srv.stop();

	return 0;
}