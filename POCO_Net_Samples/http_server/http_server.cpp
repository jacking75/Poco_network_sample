// 출처: http://codezine.jp/article/download/1064
// 간단 웹 서버

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Util/ServerApplication.h"

#include "Poco/File.h"

class NotFileHandler : public Poco::Net::HTTPRequestHandler
{
public:

	void handleRequest(Poco::Net::HTTPServerRequest& request,
		Poco::Net::HTTPServerResponse& response)
	{
		response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
		response.setContentType("text/html");

		std::ostream& ostr = response.send();
		ostr << "<html><head>";
		ostr << "<title>MiniWebServer powered by POCO</title>";
		ostr << "</head><body>Error. No Fill<br />";
		ostr << "</body></html>";
	}
};

class FileRequestHandler : public Poco::Net::HTTPRequestHandler
{
public:

	void handleRequest(Poco::Net::HTTPServerRequest& request,
		Poco::Net::HTTPServerResponse& response)
	{
		try {
			std::string fpass = ".";
			fpass += request.getURI();

			response.sendFile(fpass, "text/html");
		}
		catch (Poco::Exception&)
		{
		}
	}
};

class MiniRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
{
public:

	Poco::Net::HTTPRequestHandler*
		createRequestHandler(const Poco::Net::HTTPServerRequest& request)
	{
		std::string fpass = ".";
		fpass += request.getURI();

		Poco::File f(fpass);
		if (!f.exists() || !f.isFile()) return new NotFileHandler();

		return new FileRequestHandler();
	}
};

int main()
{
	// 클라이언트의 최대 동시 접속자 수
	int maxThreads = 1;

	// 접속 스레드 수 설정
	Poco::ThreadPool::defaultPool().addCapacity(maxThreads);

	// 설정 값 클래스
	Poco::Net::HTTPServerParams* pParams = new Poco::Net::HTTPServerParams;

	// 접속 요구 소켓 최대 수
	pParams->setMaxQueued(100);
	pParams->setMaxThreads(maxThreads);
	// 서버의 port는 9980 
	Poco::Net::ServerSocket svs(9980);

	// HTTP 서버 클래스 인스턴스
	Poco::Net::HTTPServer    srv(new MiniRequestHandlerFactory(), svs, pParams);

	// 서버 시작
	srv.start();

	getchar();

	// 서버 동작 중단
	srv.stop();

	return 0;
}