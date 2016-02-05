// √‚√≥: http://poco.roundsquare.net/2011/03/13/poconetdns/ 


#include <Poco/Net/DNS.h>
#include <Poco/Net/IPAddress.h>
#include <Poco/Net/NetException.h>

#include <string>

#include "ScopedLogMessage.h"
#include "PrepareConsoleLogger.h"

void ShowHostEntryItems(const Poco::Net::HostEntry& he, const ScopedLogMessage& msg)
{
	msg.Message(Poco::format("       name: %s", he.name()));
	for (Poco::Net::HostEntry::AddressList::const_iterator itr = he.addresses().begin(); itr != he.addresses().end(); ++itr)
	{
		msg.Message(Poco::format("  address#%d: %s",
			static_cast<int>(itr - he.addresses().begin()),
			itr->toString()));
	}
	for (Poco::Net::HostEntry::AliasList::const_iterator itr = he.aliases().begin(); itr != he.aliases().end(); ++itr)
	{
		msg.Message(Poco::format("    alias#%d: %s",
			static_cast<int>(itr - he.aliases().begin()),
			*itr));
	}
}

int main(int /*argc*/, char** /*argv*/)
{
	PrepareConsoleLogger logger(Poco::Logger::ROOT, Poco::Message::PRIO_INFORMATION);

	ScopedLogMessage msg("DNSTest ", "start", "end");

	msg.Message("Poco::Net::DNS::hostName()");
	msg.Message(Poco::format("  %s", Poco::Net::DNS::hostName()));

	const std::string hostname("www.google.com");

	Poco::Net::DNS::flushCache();

	try
	{
		msg.Message(Poco::format("Poco::Net::DNS::hostByName(\"%s\")", hostname));
		ShowHostEntryItems(Poco::Net::DNS::hostByName(hostname), msg);

		const std::string hostaddress(Poco::Net::DNS::hostByName(hostname).addresses().begin()->toString());
		const Poco::Net::IPAddress ipaddress(hostaddress);

		msg.Message(Poco::format("Poco::Net::DNS::hostByAddress(Poco::Net::IPAddress(\"%s\"))", ipaddress.toString()));
		ShowHostEntryItems(Poco::Net::DNS::hostByAddress(ipaddress), msg);

		msg.Message(Poco::format("Poco::Net::DNS::resolve(\"%s\")", hostname));
		ShowHostEntryItems(Poco::Net::DNS::resolve(hostname), msg);

		msg.Message(Poco::format("Poco::Net::DNS::resolve(\"%s\")", hostaddress));
		ShowHostEntryItems(Poco::Net::DNS::resolve(hostaddress), msg);
	}
	catch (Poco::Net::HostNotFoundException&)
	{
	}
	catch (Poco::Net::NoAddressFoundException&)
	{
	}
	catch (Poco::Net::DNSException&)
	{
	}
	catch (Poco::IOException&)
	{
	}

	return 0;
}