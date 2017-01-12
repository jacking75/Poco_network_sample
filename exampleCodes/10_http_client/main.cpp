#include <string>
#include <iostream>

#include "SimpleHttpGetter.h"
#include "SimpleHttpPoster.h"


int	Get(const std::string& path, std::ostream& ostr);
int	Post(const std::string& path, std::ostream& ostr);

int main()
{
	/*Poco::URI uri("http://poco.roundsquare.net/downloads/test.txt");
	{
		SimpleHTTPGetter getter(uri.getHost(), uri.getPort());
		getter.Get(uri.getPath(), std::cout);
	}*/

	SimpleHttpPoster poster;
	auto result = poster.Start("http://localhost:20401/Service/TestRequestStream", 1000);
	std::cout << result << std::endl;
 
	return 0;
}


int	Get(const std::string& path, std::ostream& ostr)
{
	try
	{
		const std::string myPath = ('/' == path.c_str()[0]) ? path : ("/" + path);

		Poco::Net::HTTPClientSession session(mHost, mPort);
		Poco::Net::HTTPRequest req(Poco::Net::HTTPRequest::HTTP_GET, path, Poco::Net::HTTPMessage::HTTP_1_1);
		session.sendRequest(req);
		Poco::Net::HTTPResponse res;
		std::istream& rs = session.receiveResponse(res);
		Poco::StreamCopier::copyStream(rs, ostr);
	}
	catch (Poco::Exception& exc)
	{
		return -1;
	}

	return 0;
}

int	Post(const std::string& path, std::ostream& ostr)
{
}

/*
Poco::Net::HTTPClientSession s("127.0.0.1", 9090);
Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, "/echo");

std::string body("rs=this is a random request body");
request.setContentLength(body.length());
s.sendRequest(request) << body;
*/