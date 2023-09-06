#include "ClientHandler.h"
#include "json.h"
ClientHandler::ClientHandler(int id, websocketpp::connection_hdl hdl, std::string uri)
    : m_id(id),
    m_hdl(hdl),
    m_status("Connecting"),
    m_uri(uri),
    m_server("N/A") {}

void ClientHandler::on_open(client* c, websocketpp::connection_hdl hdl) {
    m_status = "Open";
    client::connection_ptr con = c->get_con_from_hdl(hdl);
    m_server = con->get_response_header("Server");
    dataReceivedSignal = [](const std::string&) { /* Default behavior when no slot is connected */ };
}

void ClientHandler::on_fail(client* c, websocketpp::connection_hdl hdl) {
    m_status = "Failed";
    std::cout << "Failed" << std::endl;

    client::connection_ptr con = c->get_con_from_hdl(hdl);
    m_server = con->get_response_header("Server");
    m_error_reason = con->get_ec().message();
}

void ClientHandler::on_close(client* c, websocketpp::connection_hdl hdl) {
    m_status = "Closed";
    client::connection_ptr con = c->get_con_from_hdl(hdl);
    std::stringstream s;
    s << "close code: " << con->get_remote_close_code() << " ("
      << websocketpp::close::status::get_string(con->get_remote_close_code())
      << "), close reason: " << con->get_remote_close_reason();
    m_error_reason = s.str();
}

void ClientHandler::on_message(websocketpp::connection_hdl, client::message_ptr msg)
{
    json::JSON json = json::Array();
    json::JSON request = json::JSON::Load(msg->get_payload());

    std::string message = msg->get_payload();

    emitDataReceived(msg->get_payload());
    if(request["status"].ToString() != "")
    {
        std::cout << request.dump() << std::endl;
    }
}

websocketpp::connection_hdl ClientHandler::get_hdl() const {
    return m_hdl;
}

int ClientHandler::get_id() const {
    return m_id;
}

std::string ClientHandler::get_status() const {
    return m_status;
}

void ClientHandler::record_sent_message(std::string message) {
    m_messages.insert(message);
}

std::set<std::string> ClientHandler::getMessages() {
    return m_messages;
}

void ClientHandler::handleSubscriptionMessage(const std::string &message)
{
    m_subscription = message;
}

void ClientHandler::onDataReceived(const std::function<void(const std::string&)>& slot)
{
    dataReceivedSignal = slot;
}

void ClientHandler::emitDataReceived(const std::string &data)
{
    dataReceivedSignal(data);
}

