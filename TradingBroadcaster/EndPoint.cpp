#include "EndPoint.h"

EndPoint::EndPoint() : m_next_id(0)
{
    m_endpoint.clear_access_channels(websocketpp::log::alevel::all);
    m_endpoint.clear_error_channels(websocketpp::log::elevel::all);

    m_endpoint.init_asio();
    m_endpoint.start_perpetual();

    m_thread = websocketpp::lib::make_shared<websocketpp::lib::thread>(&client::run, &m_endpoint);
}

EndPoint::~EndPoint()
{
    m_endpoint.stop_perpetual();

    for (con_list::const_iterator it = m_connection_list.begin(); it != m_connection_list.end(); ++it) {
        if (it->second->get_status() != "Open") {
            // Only close open connections
            continue;
        }

        websocketpp::lib::error_code ec;
        m_endpoint.close(it->second->get_hdl(), websocketpp::close::status::going_away, "", ec);
        if (ec) {
            std::cout << "> Error closing connection " << it->second->get_id() << ": "
                      << ec.message() << std::endl;
        }
    }

    m_thread->join();
}

int EndPoint::connect(const std::string& uri)
{
    websocketpp::lib::error_code ec;

    client::connection_ptr con = m_endpoint.get_connection(uri, ec);

    if (ec) {
        std::cout << "> Connect initialization error: " << ec.message() << std::endl;
        return -1;
    }

    int new_id = m_next_id++;
    ClientHandler::ptr metadata_ptr = websocketpp::lib::make_shared<ClientHandler>(new_id, con->get_handle(), uri);
    m_connection_list[new_id] = metadata_ptr;

    con->set_open_handler(websocketpp::lib::bind(
        &ClientHandler::on_open,
        metadata_ptr,
        &m_endpoint,
        websocketpp::lib::placeholders::_1
        ));
    con->set_fail_handler(websocketpp::lib::bind(
        &ClientHandler::on_fail,
        metadata_ptr,
        &m_endpoint,
        websocketpp::lib::placeholders::_1
        ));
    con->set_close_handler(websocketpp::lib::bind(
        &ClientHandler::on_close,
        metadata_ptr,
        &m_endpoint,
        websocketpp::lib::placeholders::_1
        ));
    con->set_message_handler(websocketpp::lib::bind(
        &ClientHandler::on_message,
        metadata_ptr,
        websocketpp::lib::placeholders::_1,
        websocketpp::lib::placeholders::_2
        ));

    m_endpoint.connect(con);

    return new_id;
}

int EndPoint::close(int id, websocketpp::close::status::value code, std::string reason)
{
    websocketpp::lib::error_code ec;
    con_list::iterator metadata_it = m_connection_list.find(id);
    if (metadata_it == m_connection_list.end()) {
        std::cout << "> No connection found with id " << id << std::endl;
        return 1;
    }

    m_endpoint.close(metadata_it->second->get_hdl(), code, reason, ec);
    if (ec) {
        std::cout << "> Error initiating close: " << ec.message() << std::endl;
        return 0;
    }
    return 1;
}

void EndPoint::send(int id, std::string message)
{
    websocketpp::lib::error_code ec;

    con_list::iterator metadata_it = m_connection_list.find(id);
    if (metadata_it == m_connection_list.end()) {
        std::cout << "> No connection found with id " << id << std::endl;
        return;
    }

    m_endpoint.send(metadata_it->second->get_hdl(), message, websocketpp::frame::opcode::text, ec);
    if (ec) {
        std::cout << "> Error sending message: " << ec.message() << std::endl;
        return;
    }
}

ClientHandler::ptr EndPoint::get_metadata(int id) const
{
    con_list::const_iterator metadata_it = m_connection_list.find(id);
    if (metadata_it == m_connection_list.end()) {
        return ClientHandler::ptr();
    } else {
        return metadata_it->second;
    }
}

std::string EndPoint::getStatus(int id)
{
    con_list::const_iterator metadata_it = m_connection_list.find(id);
    if (metadata_it == m_connection_list.end()) {
        return "";
    } else {
        return metadata_it->second->get_status();
    }
}
