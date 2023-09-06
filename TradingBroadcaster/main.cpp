#include "TradingBroadcasterServer.h"
#include "Utils.h"

int main()
{
	int wsPort = 9000;
	int storageAPIPort = 3000;

	auto envVariables = Utils::parseEnvFile("TradingBroadcaster.env");

	auto wsPortIt = envVariables.find("WS_CONSUMER_PORT");
	if (wsPortIt != envVariables.end()) {
		wsPort = std::stoi(wsPortIt->second);
	}

	auto storageAPIPortIt = envVariables.find("STORAGE_API_PORT");
	if (storageAPIPortIt != envVariables.end()) {
		storageAPIPort = std::stoi(storageAPIPortIt->second);
	}

	TradingBroadcasterServer server(wsPort, storageAPIPort);
	server.run();

	return 0;
}
