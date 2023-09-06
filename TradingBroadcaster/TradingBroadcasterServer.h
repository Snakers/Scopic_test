#ifndef TRADINGBROADCASTERSERVER_H
#define TRADINGBROADCASTERSERVER_H

#include <websocketpp/server.hpp>
#include <map>
#include <set>
#include <vector>
#include <mutex>
#include <thread>
#include "StorageHttpClient.h"
#include "EndPoint.h"
#include "json.h"

class TradingBroadcasterServer {
public:
    static constexpr int DEFAULT_SLEEP_INTERVAL_MS = 1000;
    static constexpr int CONNECTION_WAIT_INTERVAL_MS = 100;

    TradingBroadcasterServer(int port, int storageAPIPort);

    void run();

private:

    void handle_clear_providers(websocketpp::connection_hdl hdl, websocketpp::server<websocketpp::config::asio>::message_ptr msg);

    void handle_add_provider(websocketpp::connection_hdl hdl, websocketpp::server<websocketpp::config::asio>::message_ptr msg);

    void handle_add_aggregator(websocketpp::connection_hdl hdl, websocketpp::server<websocketpp::config::asio>::message_ptr msg);

    void handle_clear_aggregator(websocketpp::connection_hdl hdl, websocketpp::server<websocketpp::config::asio>::message_ptr msg);

    void on_message(websocketpp::connection_hdl hdl, websocketpp::server<websocketpp::config::asio>::message_ptr msg);

    void on_close_Connection(websocketpp::connection_hdl hdl);

    void on_connection(websocketpp::connection_hdl hdl);

    void subscribeMessages(websocketpp::connection_hdl hdl, std::string channel);

    void handle_clear_prices(websocketpp::connection_hdl hdl, websocketpp::server<websocketpp::config::asio>::message_ptr msg);

    void handleError(const std::string& status, const std::string& message, websocketpp::connection_hdl hdl);
private:

    typedef websocketpp::server<websocketpp::config::asio> Server;
    std::mutex m_connectionsMutex;
    Server m_server;
    std::map<std::shared_ptr<EndPoint>, std::set<int>> m_endpoints;
    std::set<int> m_ids;
    std::map<std::string, std::set<websocketpp::connection_hdl>> m_channels;
    std::vector<std::future<void>> m_sendFutures;
    json::JSON m_broadcastArray;
    std::unique_ptr<StorageHttpClient> m_storageclient;
    int m_storagePort;
    std::set<std::string> m_symbols;
    int m_clearedPrices;
};

#endif // TRADINGBROADCASTERSERVER_H
