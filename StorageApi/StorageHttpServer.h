#ifndef STORAGEHTTPSERVER_H
#define STORAGEHTTPSERVER_H

#define ASIO_STANDALONE
#define _WEBSOCKETPP_CPP11_STRICT_

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include "Price.h"

class StorageHttpServer
{
	typedef websocketpp::server<websocketpp::config::asio> Server;
	typedef std::shared_ptr<websocketpp::connection<websocketpp::config::asio> > ConnectionPtr;
public:
	StorageHttpServer(int port);

	//this will block caller thread and start event loop of the server
	void run();
		
private:
	void onHttp(websocketpp::connection_hdl hdl);

	void handleGetSymbols(const std::string& req, ConnectionPtr con);
	void handleGetSymbolById(const std::string& req, ConnectionPtr con);

	void handleGetPrices(const std::string& req, ConnectionPtr con);
	void handleGetPriceById(const std::string& req, ConnectionPtr con);
	void handleDeletePrices(const std::string& req, ConnectionPtr con);
	void handlePostPrice(const std::string& req, ConnectionPtr con);

	websocketpp::server<websocketpp::config::asio> m_server;
	std::map<decltype(Price::m_symbol), Price> m_prices;
};

#endif // !STORAGEHTTPSERVER_H
