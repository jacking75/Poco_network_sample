#pragma once 

#include <iostream>
#include <string>

#include <Poco/URI.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTMLForm.h>
#include <Poco/StreamCopier.h>

#include "StopWatch.h"


class SimpleServerRequest
{
public:
	SimpleServerRequest() { }
	~SimpleServerRequest() {}

	bool Start(const std::string& url, int reqCount)
	{
		StopWatch secondCheck;
		secondCheck.Start();

		for(auto i = 0; i < reqCount; ++i)
		{
			Poco::URI uri(url);
			std::string resultPacket;

			if(Post(uri, resultPacket) == -1)
			{
				return false;
			}

			//std::cout << resultPacket << std::endl;
		}

		std::cout << "총 소요시간(초): " << secondCheck.End() << std::endl;

		return true;
	}
 

private:
	int	Post(Poco::URI& uri, std::string& resultBuffer)
	{
		try
		{
			std::string path = uri.getPath();
			const std::string myPath = ('/' == uri.getPath().c_str()[0]) ? uri.getPath():("/"+path);
 			
			Poco::Net::HTTPRequest req(Poco::Net::HTTPRequest::HTTP_POST, uri.getPath(), Poco::Net::HTTPMessage::HTTP_1_1);
			
			req.setContentType("application/x-www-form-urlencoded");
			Poco::Net::HTMLForm form;
			form.add("userName", "hello");
			form.add("password", "123456");
			form.prepareSubmit(req);

			Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
			std::ostream& ostr = session.sendRequest(req);
			form.write(ostr);


			Poco::Net::HTTPResponse res;
			std::istream& is = session.receiveResponse(res);
			
			std::streamsize _size = Poco::StreamCopier::copyToString( is, resultBuffer );
			return (int)_size;
		}
		catch (Poco::Exception& exc)
		{
			return -1;
		}

		return 0;
	}


	std::string		mHost;
	Poco::UInt16	mPort;
};