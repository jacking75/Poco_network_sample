#include <string>
#include <iostream>

#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTMLForm.h>
#include <Poco/StreamCopier.h>

int	Get()
{
	try
	{
		std::cout << "Get(): Req" << std::endl;


		Poco::Net::HTTPRequest req(Poco::Net::HTTPRequest::HTTP_GET, "/echo", Poco::Net::HTTPMessage::HTTP_1_1);
		req.setContentType("application/x-www-form-urlencoded");

		Poco::Net::HTMLForm form;
		form.add("userName", "hello");
		form.add("password", "123456");
		form.prepareSubmit(req);

		Poco::Net::HTTPClientSession session("127.0.0.1", 19980);
		std::ostream& ostr = session.sendRequest(req);
		form.write(ostr);


		Poco::Net::HTTPResponse res;
		std::istream& is = session.receiveResponse(res);

		std::string strResponse;
		Poco::StreamCopier::copyToString(is, strResponse);
		std::cout << "Get(): Res" << strResponse << std::endl;
	}
	catch (Poco::Exception& exc)
	{
		return -1;
	}

	return 0;
}

int	Post()
{
	try
	{
		std::cout << "Post(): Req" << std::endl;


		Poco::Net::HTTPClientSession session("127.0.0.1", 19980);
		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, "/echo");

		std::string body("rs=this is a random request body");
		request.setContentLength(body.length());
		std::ostream& ostr = session.sendRequest(request);
		ostr << body;

		Poco::Net::HTTPResponse res;
		std::istream& rs = session.receiveResponse(res);

		std::string strResponse(std::istreambuf_iterator<char>(rs), {});
		std::cout << "Get(): Res" << strResponse << std::endl;
	}
	catch (Poco::Exception& exc)
	{
		return -1;
	}

	return 0;
}


int main()
{
	Get();
	
	Post();
	 
	return 0;
}



