#include "TradingAggregatorServer.h"
#include "Utils.h"

int main(int argc, char* argv[])
{
	int wsPort = 9100;
	int storageAPIPort = 3000;

	if (argc > 1) {
		try {
			wsPort = std::stoi(argv[1]);
			storageAPIPort = std::stoi(argv[2]);
		}
		catch (std::exception& e) {
			std::cout << "[TradingAggregatorApp main] " << e.what() << std::endl;
		}
	}
	else {
		auto envVariables = Utils::parseEnvFile("TradingAggregator.env");

		auto wsPortIt = envVariables.find("WS_CONSUMER_PORT");
		if (wsPortIt != envVariables.end()) {
			wsPort = std::stoi(wsPortIt->second);
		}

		auto storageAPIPortIt = envVariables.find("STORAGE_API_PORT");
		if (storageAPIPortIt != envVariables.end()) {
			storageAPIPort = std::stoi(storageAPIPortIt->second);
		}
	}

	TradingAggregatorServer server(wsPort, storageAPIPort);
	server.run();

	return 0;
}