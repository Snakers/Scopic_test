#include "StorageHttpClient.h"

#include <asio.hpp>
#include <iostream>

StorageHttpClient::StorageHttpClient(int port)
{
	m_baseUrl = "http://127.0.0.1:" + std::to_string(port);
    m_port = port;
}

std::string StorageHttpClient::getSymbol(const std::string& symbolId)
{
    std::string resultStr;
    bool res = sendRequest("GET", "/api/symbols/" + symbolId, "", resultStr);
    std::cout << "[StorageHttpClient::getSymbol] result: " << res << std::endl;
    return resultStr;
}

std::string StorageHttpClient::postSymbol(const std::string &symbolId)
{
    std::string resultStr;
    bool res = sendRequest("POST", "/api/symbols/" + symbolId, "", resultStr);
    std::cout << "[StorageHttpClient::getSymbol] result: " << res << std::endl;
    return resultStr;
}

std::string StorageHttpClient::deleteAllPrices()
{
    std::string resultStr;
    bool res = sendRequest("DELETE", "/api/prices", "", resultStr);
    std::cout << "[StorageHttpClient::deleteAllPrices] result: " << res << std::endl;
    return resultStr;

}

bool StorageHttpClient::sendRequest(const std::string& type, const std::string& path, const std::string& body, std::string& result)
{
    try {
        asio::io_context context;

        asio::ip::tcp::resolver resolver(context);
        auto endpoints = resolver.resolve("127.0.0.1", std::to_string(m_port));

        asio::ip::tcp::socket socket(context);
        asio::connect(socket, endpoints);

        std::string request = type + " " + path + " HTTP/1.1\r\nHost: " + m_baseUrl;
        if (!body.empty()) {
            request += "\r\nContent-Type: application/json";
            request += "\r\nContent-Length: " + std::to_string(body.length());
        }
        request += "\r\n\r\n";
        if (!body.empty()) {
            request += body;
        }
        asio::write(socket, asio::buffer(request, request.size()));

        char response[4096];
        int readBytesCount = socket.read_some(asio::buffer(response, sizeof(response)));

        std::string responseStr(response, response + readBytesCount);

        auto headerEnd = responseStr.find("\r\n\r\n");
        std::string header = responseStr.substr(0, headerEnd);
        std::string responseBody = responseStr.substr(headerEnd + 4);

        auto statusCodeStart = header.find(' ') + 1;
        auto statusCodeEnd = header.find(' ', statusCodeStart);
        int statusCode = std::stoi(header.substr(statusCodeStart, statusCodeEnd - statusCodeStart));

        if (statusCode != 200) {
            std::cout << "[StorageHttpClient::sendRequest] error: received status code " << statusCode << std::endl;
            return false;
        }
        result = responseBody;
    }
    catch (std::exception& e) {
        std::cout << "[StorageHttpClient::sendRequest] error: " << e.what() << std::endl;
        return false;
    }
    return true;
}
