#include <iostream>

#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

#include <Poco/File.h>


class NotFileHandler : public Poco::Net::HTTPRequestHandler
{
public:

	void handleRequest(Poco::Net::HTTPServerRequest& request,
						Poco::Net::HTTPServerResponse& response)
	{
		response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
		response.setContentType("text/html");

		std::ostream& ostr = response.send();
		ostr << "<html>";
		ostr << "<head><title>Simple WebServer powered by POCO</title></head>";
		ostr << "<body>Error. No Fill<br />";
		ostr << "YOUR REQUEST=" << request.getURI() << "<br />";
		ostr << "YOUR ADDRESS=" << request.clientAddress().toString() << "<br />";
		ostr << "</body>";
		ostr << "<html>";
	}
};

class FileRequestHandler : public Poco::Net::HTTPRequestHandler
{
public:

	void handleRequest(Poco::Net::HTTPServerRequest& request,
					Poco::Net::HTTPServerResponse& response)
	{
		try 
		{
			std::cout << "FileRequestHandler: " << request.getURI() << std::endl;

			std::string fpass = "." + request.getURI();			
			response.sendFile(fpass, "text/html");
		}
		catch (Poco::Exception& exc)
		{
			std::cout << "FileRequestHandler: " << exc.displayText() << std::endl;
		}
	}
};

class SimpleRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
{
public:

	Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request)
	{
		std::cout << "SimpleRequestHandlerFactory: " << request.getURI() << std::endl;

		std::string fpass = "." + request.getURI();

		Poco::File f(fpass);

		if (!f.exists() || !f.isFile()) {
			return new NotFileHandler();
		}

		return new FileRequestHandler();
	}
};

int main()
{
	// 클라이언트의 최대 동시 접속자 수
	int maxThreads = 1;

	// 접속 스레드 풀 수 설정
	Poco::ThreadPool::defaultPool().addCapacity(maxThreads);


	// 설정 값 클래스
	Poco::Net::HTTPServerParams* pParams = new Poco::Net::HTTPServerParams;

	// 요구 대기 최대 수
	pParams->setMaxQueued(100);

	// 최대 동시 처리 수
	pParams->setMaxThreads(maxThreads);
	
	
	// 서버의 port는 19980 
	Poco::Net::ServerSocket svs(19980);

	// HTTP 서버 클래스 인스턴스
	Poco::Net::HTTPServer    srv(new SimpleRequestHandlerFactory(), svs, pParams);

	
	// 서버 시작
	srv.start();

	std::cout << "Port 19980으로 서버 시작" << std::endl;

	getchar();

	// 서버 동작 중단
	srv.stop();

	return 0;
}