#include <iostream>

#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/HTMLForm.h>



class FormRequestHandler : public Poco::Net::HTTPRequestHandler
{
public:

	void handleRequest(Poco::Net::HTTPServerRequest& request,
					Poco::Net::HTTPServerResponse& response)
	{
		try 
		{
			std::cout << "FormRequestHandler: " << request.getURI() << std::endl;

			Poco::Net::HTMLForm form(request, request.stream());
			if (!form.empty())
			{
				Poco::Net::NameValueCollection::ConstIterator itr = form.begin();
				Poco::Net::NameValueCollection::ConstIterator end = form.end();
				for (; itr != end; ++itr)
				{
					std::cout << itr->first << ", " << itr->second << std::endl;
				}
			}

			
			response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_OK);
			const char * data = "Ok.\n";
			response.sendBuffer(data, strlen(data));
		}
		catch (Poco::Exception& exc)
		{
			std::cout << "FormRequestHandler: " << exc.displayText() << std::endl;
		}
	}
};

class SimpleRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
{
public:

	Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request)
	{
		std::cout << "SimpleRequestHandlerFactory: " << request.getURI() << std::endl;

		return new FormRequestHandler();
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