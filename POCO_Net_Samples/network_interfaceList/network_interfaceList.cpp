// √‚√≥: http://poco.roundsquare.net/2011/02/19/poconetnetworkinterface/

#include <Poco/Net/NetworkInterface.h>
#include <Poco/Net/IPAddress.h>

#include <string>

#include "ScopedLogMessage.h"
#include "PrepareConsoleLogger.h"

int main(int /*argc*/, char** /*argv*/)
{
	PrepareConsoleLogger logger(Poco::Logger::ROOT, Poco::Message::PRIO_INFORMATION);

	ScopedLogMessage msg("NetworkInterfaceTest ", "start", "end");

	Poco::Net::NetworkInterface::NetworkInterfaceList list = Poco::Net::NetworkInterface::list();
	int index = 0;
	if (!list.empty())
	{
		for (Poco::Net::NetworkInterface::NetworkInterfaceList::const_iterator itr = list.begin(); itr != list.end(); ++itr)
		{
			msg.Message(Poco::format("#%d %s (%s)", ++index, itr->name(), itr->displayName()));
			msg.Message(Poco::format("     %s %s (%s)"
				, std::string(itr->supportsIPv4() ? "IPv4" : (itr->supportsIPv6() ? "IPv6" : "    "))
				, itr->address().toString()
				, itr->broadcastAddress().toString()));
			std::string addressInfo;
			addressInfo += std::string((Poco::Net::IPAddress::IPv4 == itr->address().family()) ? "IPv4" : "IPv6");
			addressInfo += itr->address().isBroadcast() ? " isBroadcast" : "";
			addressInfo += itr->address().isGlobalMC() ? " isGlobalMC" : "";
			addressInfo += itr->address().isIPv4Compatible() ? " isIPv4Compatible" : "";
			addressInfo += itr->address().isIPv4Mapped() ? " isIPv4Mapped" : "";
			addressInfo += itr->address().isLinkLocal() ? " isLinkLocal" : "";
			addressInfo += itr->address().isLinkLocalMC() ? " isLinkLocalMC" : "";
			addressInfo += itr->address().isLoopback() ? " isLoopback" : "";
			addressInfo += itr->address().isMulticast() ? " isMulticast" : "";
			addressInfo += itr->address().isNodeLocalMC() ? " isNodeLocalMC" : "";
			addressInfo += itr->address().isOrgLocalMC() ? " isOrgLocalMC" : "";
			addressInfo += itr->address().isSiteLocal() ? " isSiteLocal" : "";
			addressInfo += itr->address().isSiteLocalMC() ? " isSiteLocalMC" : "";
			addressInfo += itr->address().isUnicast() ? " isUnicast" : "";
			addressInfo += itr->address().isWellKnownMC() ? " isWellKnownMC" : "";
			addressInfo += itr->address().isWildcard() ? " isWildcard" : "";
			msg.Message(Poco::format("    [%s]", addressInfo));
		}
	}

	return 0;
}
