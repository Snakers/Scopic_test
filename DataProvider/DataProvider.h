#ifndef DATAPROVIDER_H
#define DATAPROVIDER_H


#define ASIO_STANDALONE
#define _WEBSOCKETPP_CPP11_STRICT_

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include "DataProviderMessage.h"

class DataProvider
{
	typedef websocketpp::server<websocketpp::config::asio> Server;
public:
	DataProvider(int port);

	//this will block caller thread and start event loop of the server
	void run();

private:
	void onConnection(websocketpp::connection_hdl hdl);
	void onCloseConnection(websocketpp::connection_hdl hdl);
	void onMessage(websocketpp::connection_hdl hdl, Server::message_ptr msg);

	Server m_server;
	std::vector<DataProviderMessage> m_messages;
	std::vector<std::future<void>> m_sendFutures;
};


#endif // !DATAPROVIDER_H
