#include "TradingBroadcasterServer.h"
#include "json.h"

TradingBroadcasterServer::TradingBroadcasterServer(int port, int storageAPIPort)
    : m_broadcastArray(json::Array()), m_storagePort{storageAPIPort}, m_clearedPrices{false}
{
    m_server.set_access_channels(websocketpp::log::alevel::all);
    m_server.clear_access_channels(websocketpp::log::alevel::frame_payload);

    m_server.init_asio();
    m_server.set_message_handler(std::bind(&TradingBroadcasterServer::on_message, this, std::placeholders::_1, std::placeholders::_2));
    m_server.set_close_handler(std::bind(&TradingBroadcasterServer::on_close_Connection, this, std::placeholders::_1));
    m_server.set_open_handler(std::bind(&TradingBroadcasterServer::on_connection, this, std::placeholders::_1));
    m_storageclient = std::make_unique<StorageHttpClient>(m_storagePort);

    m_server.listen(port);
}

void TradingBroadcasterServer::run()
{
    m_server.start_accept();

    std::cout << "Data TradingBroadcasterServer is running" << std::endl;
    m_server.run();
}

void TradingBroadcasterServer::on_connection(websocketpp::connection_hdl hdl)
{

}

/***************************** Note: *****************************
****************************** All endpoints subscribed*****************************
****************************** The reason is there is no consumer handler*****************************
***************************** We can publish message from "Simple websocket Client"*****************************
*/
void TradingBroadcasterServer::subscribeMessages(websocketpp::connection_hdl hdl, std::string channel)
{
    std::future<void> future = std::async(std::launch::async, [this, channel, hdl]() {

        for (auto& endpoint : m_endpoints) {
            std::set<int> clientIds = endpoint.second;

            for (int clientId : clientIds) {

                ClientHandler::ptr clientHandler = endpoint.first->get_metadata(clientId);
                clientHandler->onDataReceived([&, channel, hdl](const std::string& data) {
                    websocketpp::server<websocketpp::config::asio>::connection_ptr con = m_server.get_con_from_hdl(hdl);
                    json::JSON status = json::JSON::Load(data);


                    if(channel == "broadcast")
                    {
                        if(m_clearedPrices) {
                            for(auto &symbol : m_symbols)
                                m_storageclient->postSymbol(symbol);
                            m_clearedPrices =false;
                        }
                        json::JSON response = json::JSON::Load(data);
                        m_broadcastArray.append(response);
                        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                        if(m_broadcastArray.size() == 2) {
                            con->send(m_broadcastArray.dump());
                            m_broadcastArray = json::Array();
                        }
                    }

                    else if(channel == "latest-price")
                    {
                        if(m_clearedPrices) {
                            for(auto &symbol : m_symbols)
                                m_storageclient->postSymbol(symbol);
                            m_clearedPrices = false;
                        }

                        json::JSON response = json::JSON::Load(data);
                        std::string newResponse = "{\n";
                        newResponse += "  \"price\" : " + std::to_string(response["price"].ToFloat()) + ",\n";
                        newResponse += "  \"quantity\" : " + std::to_string(response["quantity"].ToInt()) + ",\n";
                        newResponse += "  \"symbol\" : \"" + response["symbol"].ToString() + "\",\n";
                        newResponse += "  \"lastprice\" : true,\n";
                        newResponse += "  \"timestamp\" : " + std::to_string(response["timestamp"].ToInt()) + "\n";
                        newResponse += "}";

                        // Send the
                        con->send(newResponse);
                    }
                    else if(channel == "unsubscribe")
                    {
                        m_storageclient->deleteAllPrices();
                        m_clearedPrices = true;

                        handleError("processed", "", hdl);
                    }
                    // Special case return of add-provider.
                    else if(!status["status"].ToString().empty())
                    {
                        json::JSON response = json::JSON::Load(data);
                        websocketpp::server<websocketpp::config::asio>::connection_ptr con = m_server.get_con_from_hdl(hdl);
                        con->send(response.dump());
                    }

                    std::cout << status.dump() << std::endl;
                });
            }
        }
    });

    m_sendFutures.push_back(std::move(future));
}

void TradingBroadcasterServer::handle_clear_prices(websocketpp::connection_hdl hdl, Server::message_ptr msg)
{
    json::JSON request = json::JSON::Load(msg->get_payload());

    for (const auto& pair : m_endpoints) {
        for (int id : pair.second) {
            pair.first->send(id, msg->get_payload());
        }
    }
    m_clearedPrices = true;
}

void TradingBroadcasterServer::on_close_Connection(websocketpp::connection_hdl hdl)
{
    std::lock_guard<std::mutex> lock(m_connectionsMutex);
    // Todo:: i need to close the connections.
    //    try {
    //        json::JSON response = json::Object();
    //        response["action"] = "close-connection";
    //        for (const auto& pair : m_endpoints) {
    //            for (int id : pair.second) {
                //Todo:: close
    //            }
    //        }
    //        std::cout << "Connection closed" << std::endl;
    //    } catch (const std::exception& e) {
    //        std::cerr << "Error: " << e.what() << std::endl;
    //    }

}

void TradingBroadcasterServer::on_message(websocketpp::connection_hdl hdl, Server::message_ptr msg)
{
    try {
        json::JSON request = json::JSON::Load(msg->get_payload());
        std::cout << request.dump() <<std::endl;

        if(request["action"].ToString() == "subscribe" ||
            request["action"].ToString() == "unsubscribe" ||
            !request["status"].ToString().empty())
        {
            if(!request["status"].ToString().empty())
            {
                subscribeMessages(hdl, request.dump());

            }
            std::string channel = request["channel"].ToString();
            subscribeMessages(hdl, channel);
        }
        if(request["action"].ToString() == "add-trading-aggregator" )
        {
            handle_add_aggregator(hdl, msg);
        }

        if(request["action"].ToString() == "clear-trading-aggregators")
        {
            handle_clear_aggregator(hdl, msg);
        }

        if (request["action"].ToString() == "add-provider") {
            handle_add_provider(hdl, msg);
        }

        if (request["action"].ToString() == "clear-providers") {
            handle_clear_prices(hdl, msg);
        }

        if (request["action"].ToString() == "clear-prices") {
            handle_clear_providers(hdl, msg);

        }

        std::cout << msg->get_payload() << std::endl;
    } catch (websocketpp::exception& e) {

    }
}

void TradingBroadcasterServer::handle_add_aggregator(websocketpp::connection_hdl hdl, Server::message_ptr msg)
{
    try {
        std::lock_guard<std::mutex> lock(m_connectionsMutex);

        json::JSON request = json::JSON::Load(msg->get_payload());

        json::JSON response = json::Object();

        websocketpp::server<websocketpp::config::asio>::connection_ptr con = m_server.get_con_from_hdl(hdl);

        int clientId {-1};

        if (m_endpoints.empty()) {
            auto endpoint = std::make_shared<EndPoint>();
            clientId = endpoint->connect(request["host"].ToString());
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            m_ids.insert(clientId);
            m_endpoints[endpoint].insert(clientId);
        } else {
            for (auto& pair : m_endpoints) {
                auto endpoint = pair.first;
                clientId = endpoint->connect(request["host"].ToString());
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                m_ids.insert(clientId);
                pair.second.insert(clientId);
                endpoint->connect(request["host"].ToString());
            }
        }

        if(clientId != -1)
            handleError("processed", "connected to " + request["host"].ToString(), hdl);
        else
            handleError("processed", "error connecting to " + request["host"].ToString(), hdl);

    } catch (websocketpp::exception& e) {
    }
}

void TradingBroadcasterServer::handle_clear_aggregator(websocketpp::connection_hdl hdl, Server::message_ptr msg)
{
    websocketpp::server<websocketpp::config::asio>::connection_ptr con = m_server.get_con_from_hdl(hdl);

    bool allConnectionsClosed = false;

    {

        for (const auto& pair : m_endpoints)
            for (int id : pair.second)
                if(pair.first->close(id, websocketpp::close::status::normal, "Clear Aggregators"))
                {
                    allConnectionsClosed = true;
                    break;
                }
    }


    // Wait for the flag to become true, indicating that all connections are closed
    while (!allConnectionsClosed)
        std::this_thread::sleep_for(std::chrono::milliseconds(CONNECTION_WAIT_INTERVAL_MS)); // Adjust the sleep duration as needed

    json::JSON response = json::Object();
    response["status"] = "";

    if(allConnectionsClosed)
    {
        response["status"] = "processed";
        con->send(response.dump());
        m_endpoints.clear();
        m_ids.clear();
    }
    else
    {
        response["status"] = "not processed";
        con->send(response.dump());
    }
}

void TradingBroadcasterServer::handle_add_provider(websocketpp::connection_hdl hdl, Server::message_ptr msg)
{
    std::lock_guard<std::mutex> lock(m_connectionsMutex);

    if (m_endpoints.size() <= 0)
        return;

    json::JSON requestMSG = json::JSON::Load(msg->get_payload());

    for (auto& symbol : requestMSG["symbols"].ArrayRange())
        m_symbols.insert(symbol.ToString());

    for (const auto& pair : m_endpoints) {
        for (int id : pair.second) {
            for (auto& target : m_ids) {
                if (target == id) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(CONNECTION_WAIT_INTERVAL_MS));
                    pair.first->send(id, msg->get_payload());
                    std::this_thread::sleep_for(std::chrono::milliseconds(DEFAULT_SLEEP_INTERVAL_MS));
                }
            }
        }
    }
}
void TradingBroadcasterServer::handle_clear_providers(websocketpp::connection_hdl hdl, Server::message_ptr msg)
{
    json::JSON request = json::JSON::Load(msg->get_payload());

    for (const auto& pair : m_endpoints) {
        for (int id : pair.second) {
            pair.first->send(id, msg->get_payload());
        }
    }
}

void TradingBroadcasterServer::handleError(const std::string& status, const std::string& message, websocketpp::connection_hdl hdl)
{
    json::JSON response = json::Object();
    response["status"] = status;
    if(!message.empty())
        response["message"] = message;

    websocketpp::server<websocketpp::config::asio>::connection_ptr con = m_server.get_con_from_hdl(hdl);

    try {
        con->send(response.dump());
    } catch (websocketpp::exception& e) {
        // Error sending the error response
    }
}
