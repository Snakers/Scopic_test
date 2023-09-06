#ifndef TRADINGAGGREGATORSERVER_H
#define TRADINGAGGREGATORSERVER_H

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <set>
#include <map>
#include <mutex>
#include <vector>
#include <atomic>
#include <future>
#include <iostream>
#include "EndPoint.h"
#include "StorageHttpClient.h"

class TradingAggregatorServer {
public:
    static constexpr int DEFAULT_SLEEP_INTERVAL_MS = 1000;
    static constexpr int CONNECTION_WAIT_INTERVAL_MS = 100;

    TradingAggregatorServer(int port, int storageAPIPort);

    void run();

private:

    void onConnection(websocketpp::connection_hdl hdl);

    void onCloseConnection(websocketpp::connection_hdl hdl);

    void onMessage(websocketpp::connection_hdl hdl, websocketpp::server<websocketpp::config::asio>::message_ptr msg);

    void handle_add_provider(websocketpp::connection_hdl hdl, websocketpp::server<websocketpp::config::asio>::message_ptr msg);

    void handle_clear_provider(websocketpp::connection_hdl hdl);

    void sendMessages(websocketpp::connection_hdl hdl, int clientId);

    void handleError(const std::string& status, const std::string& message, websocketpp::connection_hdl hdl);

    void handle_clear_prices();

private:
    websocketpp::server<websocketpp::config::asio> m_server;
    EndPoint m_endpoint;
    std::mutex m_connectionsMutex;
    std::vector<int> m_client_ids;
    std::atomic_bool m_close;
    std::vector<std::future<void>> m_sendFutures;
    std::set<std::string> m_symbols;
    std::map<websocketpp::connection_hdl, std::vector<int>, std::owner_less<websocketpp::connection_hdl>> m_connections; // Add this line
    std::unique_ptr<StorageHttpClient> m_storageclient;
    int m_storagePort;
};

#endif // TRADINGAGGREGATORSERVER_H
