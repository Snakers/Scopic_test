#include "Ai.h"
#include "DataProvider.h"
#include "Utils.h"

int main(int argc, char* argv[])
{
	Ai().ai();
	
	int port = 9001;

	if (argc > 1) {
		try {
			port = std::stoi(argv[1]);
		}
		catch (std::exception& e) {
			std::cout << "[DataProviderApp main] " << e.what() << std::endl;
		}
	}
	else {
		auto envVariables = Utils::parseEnvFile("DataProvider.env");

		auto portIt = envVariables.find("WS_DATA_PROVIDER_PORT");
		if (portIt != envVariables.end()) {
			port = std::stoi(portIt->second);
		}
	}

	DataProvider provider(port);
	provider.run();

	return 0;
}
