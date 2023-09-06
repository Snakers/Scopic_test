#ifndef STORAGEHTTPCLIENT_H
#define STORAGEHTTPCLIENT_H

#include <string>

class StorageHttpClient
{
public:
	StorageHttpClient(int port);

	//this is sample request, which demonstrates the usage of "sendRequest" private function
    std::string getSymbol(const std::string& symbolId);
    std::string postSymbol(const std::string& symbolId);
    std::string deleteAllPrices();

private:
	bool sendRequest(const std::string& type, const std::string& path, const std::string& body, std::string& result);

	std::string m_baseUrl;
	int m_port;
};

#endif // !STORAGEHTTPCLIENT_H
