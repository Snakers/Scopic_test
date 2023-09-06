#include "TradingAggregatorServer.h"
#include "json.h"

TradingAggregatorServer::TradingAggregatorServer(int port, int storageAPIPort)
    : m_close(false), m_storagePort(storageAPIPort) {
    m_server.set_access_channels(websocketpp::log::alevel::all);
    m_server.clear_access_channels(websocketpp::log::alevel::frame_payload);

    m_server.init_asio();
    m_server.listen(port);

    m_server.set_open_handler(std::bind(&TradingAggregatorServer::onConnection, this, std::placeholders::_1));
    m_server.set_close_handler(std::bind(&TradingAggregatorServer::onCloseConnection, this, std::placeholders::_1));
    m_server.set_message_handler(std::bind(&TradingAggregatorServer::onMessage, this, std::placeholders::_1, std::placeholders::_2));
    m_storageclient = std::make_unique<StorageHttpClient>(m_storagePort);
}

void TradingAggregatorServer::run() {
    m_server.start_accept();
    std::cout << "Data Aggregation is running" << std::endl;
    m_server.run();
}

void TradingAggregatorServer::onConnection(websocketpp::connection_hdl hdl) {
    // Handle new connections here
}

void TradingAggregatorServer::onCloseConnection(websocketpp::connection_hdl hdl) {
    handle_clear_provider(hdl);
}

void TradingAggregatorServer::onMessage(websocketpp::connection_hdl hdl, websocketpp::server<websocketpp::config::asio>::message_ptr msg) {
    json::JSON response = json::JSON::Load(msg->get_payload());

    if (response["action"].ToString() == "add-provider") {
        handle_add_provider(hdl, msg);
    }

    else if (response["action"].ToString() == "clear-providers") {
        handle_clear_provider(hdl);
    }

    else if(response["action"].ToString() == "clear-prices")
    {
        handle_clear_prices();
    }

    else if (response["action"].ToString() == "subscribe" && response["channel"].ToString() == "broadcast") {
        // Handle subscriptions here
    }
}

void TradingAggregatorServer::handle_add_provider(websocketpp::connection_hdl hdl, websocketpp::server<websocketpp::config::asio>::message_ptr msg) {
    json::JSON requestMSG = json::JSON::Load(msg->get_payload());

    std::lock_guard<std::mutex> lock(m_connectionsMutex);

    for (auto& symbol : requestMSG["symbols"].ArrayRange()) {
        m_symbols.insert(symbol.ToString());
        std::this_thread::sleep_for(std::chrono::milliseconds(CONNECTION_WAIT_INTERVAL_MS));
        m_storageclient->getSymbol(symbol.ToString());
    }

    int clientId = m_endpoint.connect(requestMSG["host"].ToString());

    {
        m_connections[hdl].push_back(clientId);
    }

    while (m_endpoint.getStatus(clientId) != "Open") {
        std::this_thread::sleep_for(std::chrono::milliseconds(CONNECTION_WAIT_INTERVAL_MS));
    }

    m_client_ids.push_back(clientId);

    json::JSON response = json::Object();
    response["status"] = "";
    response["message"] = "";

    websocketpp::server<websocketpp::config::asio>::connection_ptr con = m_server.get_con_from_hdl(hdl);

    try {
        if(clientId != -1) {
//            handleError("processed","connected to " + requestMSG["host"].ToString(), hdl);
            response["status"] = "processed";
            response["message"] = "connecting to " + requestMSG["host"].dump();
            con->send(response.dump());
        }
        else {
            response["status"] = "not processed";
            response["message"] = "error connecting to " + requestMSG["host"].dump();
            con->send(response.dump());
        }
    } catch (websocketpp::exception& e) {

    }

    m_close = false;

    sendMessages(hdl, clientId);

}

void TradingAggregatorServer::handle_clear_provider(websocketpp::connection_hdl hdl)
{
    bool allConnectionsClosed = true; // Flag to track if all connections are closed
    std::lock_guard<std::mutex> lock(m_connectionsMutex);
    m_close = true;
    websocketpp::server<websocketpp::config::asio>::connection_ptr con = m_server.get_con_from_hdl(hdl);

    for (auto& client_id : m_client_ids) {
        std::this_thread::sleep_for(std::chrono::milliseconds(DEFAULT_SLEEP_INTERVAL_MS));
        if (!m_endpoint.close(client_id, websocketpp::close::status::normal, "clear-Providers"))
        {
            allConnectionsClosed = false;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(DEFAULT_SLEEP_INTERVAL_MS));

    }

    m_client_ids.clear();

    // Wait for the flag to become true, indicating that all connections are closed
    while (!allConnectionsClosed) {
        std::this_thread::sleep_for(std::chrono::milliseconds(CONNECTION_WAIT_INTERVAL_MS)); // Adjust the sleep duration as needed
    }

    if (allConnectionsClosed)
        handleError("procceed", "", hdl);
    else
        handleError("not procceed", "", hdl);
}

void TradingAggregatorServer::sendMessages(websocketpp::connection_hdl hdl, int clientId)
{
    std::future<void> future = std::async(std::launch::async, [this, hdl, clientId]{
        websocketpp::server<websocketpp::config::asio>::connection_ptr con = m_server.get_con_from_hdl(hdl);

        ClientHandler::ptr clientHandler = m_endpoint.get_metadata(clientId);
        clientHandler->onDataReceived([&, hdl](const std::string& data) {
            websocketpp::server<websocketpp::config::asio>::connection_ptr con = m_server.get_con_from_hdl(hdl);
            json::JSON response = json::JSON::Load(data);

            con->send(response.dump());
        });

    });
    m_sendFutures.push_back(std::move(future));
}

void TradingAggregatorServer::handleError(const std::string& status, const std::string& message, websocketpp::connection_hdl hdl)
{
    json::JSON response = json::Object();
    response["status"] = status;
    if(!message.empty())
        response["message"] = message;

    websocketpp::server<websocketpp::config::asio>::connection_ptr con = m_server.get_con_from_hdl(hdl);
    std::cout << response.dump() << std::endl;

    try {
        con->send(response.dump());
    } catch (websocketpp::exception& e) {
        // Error sending the error response
    }
}

void TradingAggregatorServer::handle_clear_prices()
{
    std::lock_guard<std::mutex> lock(m_connectionsMutex);

    std::string response = m_storageclient->deleteAllPrices();

    std::cout << "Response: " << response << std::endl;
}
