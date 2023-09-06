#include "DataProvider.h"

#define TIME_TO_WAIT_BEFORE_SENDING_FIRST_MESSAGE  1000
#define TIME_TO_WAIT_BEFORE_SENDING_NEW_MESSAGE  4000
#define NUMBER_OF_MESSAGE_SEND_INTERATIONS 100

DataProvider::DataProvider(int port)
{
    m_server.set_access_channels(websocketpp::log::alevel::all);
    m_server.clear_access_channels(websocketpp::log::alevel::frame_payload);

    m_server.init_asio();

    m_server.set_open_handler(std::bind(&DataProvider::onConnection, this, std::placeholders::_1));
    m_server.set_close_handler(std::bind(&DataProvider::onCloseConnection, this, std::placeholders::_1));
    m_server.set_message_handler(std::bind(&DataProvider::onMessage, this, std::placeholders::_1, std::placeholders::_2));

    m_server.listen(port);

    std::cout << "[DataProvider] Data provider is created on port: " << port << std::endl;

    m_messages.push_back(DataProviderMessage("a631dc6c-ee85-458d-80d7-50018aedfbad", 10.58, 500, 0));
    m_messages.push_back(DataProviderMessage("9e8bff74-50cd-4d80-900c-b5ce3bf371ee", 18.58, 1500, 1));
    m_messages.push_back(DataProviderMessage("a631dc6c-ee85-458d-80d7-50018aedfbad", 11.0, 1000, -500));
    m_messages.push_back(DataProviderMessage("a631dc6c-ee85-458d-80d7-50018aedfbad", 15.0, 500, 2));
    m_messages.push_back(DataProviderMessage("4", 9.0, 1000, 3));


    //    createSymbolMapObject(Symbol("257f9954-bd82-443d-8c6c-3ba81470f76c", "META")),
//        createSymbolMapObject(Symbol("ce38cfe2-dd68-406d-b6e9-adf79b449c15", "UBER"))

}

void DataProvider::run()
{
    m_server.start_accept();

    std::cout << "Data provider is running" << std::endl;
    m_server.run();
}

void DataProvider::onConnection(websocketpp::connection_hdl hdl)
{
    std::cout << "New connection established" << std::endl;

    std::future<void> future = std::async(std::launch::async, [this, hdl]() {

        Server::connection_ptr con = m_server.get_con_from_hdl(hdl);

        std::this_thread::sleep_for(std::chrono::milliseconds(TIME_TO_WAIT_BEFORE_SENDING_FIRST_MESSAGE));
        for (int i = 0; i < NUMBER_OF_MESSAGE_SEND_INTERATIONS; ++i) {
            for (auto& message : m_messages) {
                message.updateTimestamp();
                con->send(message.toJson());
                std::this_thread::sleep_for(std::chrono::milliseconds(TIME_TO_WAIT_BEFORE_SENDING_NEW_MESSAGE));
            }
        }

    });
    m_sendFutures.push_back(std::move(future));
}

void DataProvider::onCloseConnection(websocketpp::connection_hdl hdl)
{
    std::cout << "Connection closed" << std::endl;
}

void DataProvider::onMessage(websocketpp::connection_hdl hdl, Server::message_ptr msg)
{
    std::cout << "Received message: " << msg->get_payload() << std::endl;
}
