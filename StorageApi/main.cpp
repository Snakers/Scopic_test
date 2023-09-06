#include "StorageHttpServer.h"
#include "Utils.h"

int main() 
{
	int port = 3000;

	auto envVariables = Utils::parseEnvFile("StorageAPI.env");

	auto portIt = envVariables.find("STORAGE_API_PORT");
	if (portIt != envVariables.end()) {
		port = std::stoi(portIt->second);
	}

	StorageHttpServer server(port);

	std::cout << "Running the server on port: " << port << std::endl;
	server.run();

	return 0;
}