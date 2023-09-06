#ifndef ENDPOINT_H
#define ENDPOINT_H

#include "ClientHandler.h"
#include <map>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>
#include <string>
#include <iostream>
#include <chrono>
#include <future>
#include <vector>
#include <set>

typedef websocketpp::client<websocketpp::config::asio_client> client;

class EndPoint {
public:

    EndPoint();

    ~EndPoint();

    int connect(const std::string& uri);

    int close(int id, websocketpp::close::status::value code, std::string reason);

    void send(int id, std::string message);

    ClientHandler::ptr get_metadata(int id) const;

    std::string getStatus(int id);

private:
    typedef std::map<int, ClientHandler::ptr> con_list;

    client m_endpoint;

    websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_thread;

    con_list m_connection_list;

    int m_next_id;
};

#endif
