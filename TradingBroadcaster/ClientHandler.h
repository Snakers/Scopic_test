#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <string>
#include <vector>
#include <set>
#include "json.h"

typedef websocketpp::client<websocketpp::config::asio_client> client;

using DataReceivedSignal = std::function<void(std::string&)>;

class ClientHandler {
public:
    typedef websocketpp::lib::shared_ptr<ClientHandler> ptr;

    ClientHandler(int id, websocketpp::connection_hdl hdl, std::string uri);

    void on_open(client* c, websocketpp::connection_hdl hdl);

    void on_fail(client* c, websocketpp::connection_hdl hdl);

    void on_close(client* c, websocketpp::connection_hdl hdl);

    void on_message(websocketpp::connection_hdl, client::message_ptr msg);

    websocketpp::connection_hdl get_hdl() const;

    int get_id() const;

    std::string get_status() const;

    void record_sent_message(std::string message);

    std::set<std::string> getMessages();

    void handleSubscriptionMessage(const std::string& message);

    void onDataReceived(const std::function<void(const std::string&)>& slot);

    void emitDataReceived(const std::string&);

private:
    int m_id;
    websocketpp::connection_hdl m_hdl;
    std::string m_status;
    std::string m_uri;
    std::string m_server;
    std::string m_error_reason;
    json::JSON m_uniqueData;
    std::set<std::string> m_messages;
    std::string m_subscription;
    std::function<void(const std::string&)> dataReceivedSignal;
};


#endif // CLIENTHANDLER_H
