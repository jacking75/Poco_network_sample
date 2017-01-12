#pragma once 

#include <Poco/URI.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/StreamCopier.h>

class SimpleHttpGetter
{
public:
	SimpleHttpGetter(const std::string& host, Poco::UInt16 port = Poco::Net::HTTPSession::HTTP_PORT) : mHost(host), mPort(port)
	{
	}

	~SimpleHttpGetter()
	{
	}

	int	Get(const std::string& path, std::ostream& ostr)
	{
		try
		{
			const std::string myPath = ('/' == path.c_str()[0]) ? path:("/"+path);
 
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
 
private:
	SimpleHttpGetter();
	SimpleHttpGetter(const SimpleHttpGetter&);
	SimpleHttpGetter& operator = (const SimpleHttpGetter&);
 
	std::string		mHost;
	Poco::UInt16	mPort;
};