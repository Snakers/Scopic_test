#include "StorageHttpServer.h"

#include "Data.h"
#include "json.h"

StorageHttpServer::StorageHttpServer(int port)
{
	m_server.set_http_handler(std::bind(&StorageHttpServer::onHttp, this, std::placeholders::_1));

	m_server.init_asio();
	m_server.listen(port);
}

void StorageHttpServer::run()
{
	m_server.start_accept();
	m_server.run();
}

void StorageHttpServer::onHttp(websocketpp::connection_hdl hdl)
{
	auto con = m_server.get_con_from_hdl(hdl);
	std::string reqMethod = con->get_request().get_method();
	std::string req = con->get_request().get_uri();
	
	std::cout << "[StorageHttpServer::onHttp] new request: method: " << reqMethod << std::endl;

	if (reqMethod == "GET"  && req == "/api/symbols") {
		handleGetSymbols(req, con);
	}
	// Handle /symbols/:id
	else if (reqMethod == "GET" && req.substr(0, 13) == "/api/symbols/") {
		handleGetSymbolById(req, con);
	}
	else if (reqMethod == "GET" && req == "/api/prices") {
		handleGetPrices(req, con);
	}
	else if (reqMethod == "GET" && req.substr(0, 12) == "/api/prices/") {
		handleGetPriceById(req, con);
	}
	else if (reqMethod == "DELETE" && req == "/api/prices") {
		handleDeletePrices(req, con);
	}
	else if (reqMethod == "POST" && req.substr(0, 12) == "/api/prices/") {
		handlePostPrice(req, con);
	}
}

void StorageHttpServer::handleGetSymbols(const std::string& req, ConnectionPtr con)
{
	json::JSON obj = json::Array();
	for (auto it = symbols.begin(); it != symbols.end(); ++it) {
		obj.append(it->second.toJsonObject());
	}

	con->set_body(obj.dump());
	con->set_status(websocketpp::http::status_code::ok);
}

void StorageHttpServer::handleGetSymbolById(const std::string& req, ConnectionPtr con)
{
	// Extract id from the path
	std::string id = req.substr(13);

	auto it = symbols.find(id);
	if (it != symbols.end()) {
		con->set_body(it->second.toJson());
		con->set_status(websocketpp::http::status_code::ok);
	}
	else {
		// Symbol not found
		con->set_status(websocketpp::http::status_code::not_found);
	}
}

void StorageHttpServer::handleGetPrices(const std::string& req, ConnectionPtr con)
{
	json::JSON obj = json::Array();
	for (auto it = m_prices.begin(); it != m_prices.end(); ++it) {
		obj.append(it->second.toJsonObject());
	}

	con->set_body(obj.dump());
	con->set_status(websocketpp::http::status_code::ok);
}

void StorageHttpServer::handleGetPriceById(const std::string& req, ConnectionPtr con)
{
	// Extract id from the path
	std::string id = req.substr(12);

	if (symbols.find(id) == symbols.end()) {
		con->set_status(websocketpp::http::status_code::not_found, "symbol not found");
		return;
	}

	auto it = m_prices.find(id);

	if (it == m_prices.end()) {
		con->set_status(websocketpp::http::status_code::not_found, "price not found");
		return;
	}

	con->set_body(it->second.toJson());
	con->set_status(websocketpp::http::status_code::ok);
}

void StorageHttpServer::handleDeletePrices(const std::string& req, ConnectionPtr con)
{
	m_prices.clear();
	con->set_status(websocketpp::http::status_code::ok);
}

void StorageHttpServer::handlePostPrice(const std::string& req, ConnectionPtr con)
{
	std::string id = req.substr(12);
	auto symbolIt = symbols.find(id);
	if (symbolIt == symbols.end()) {
		con->set_status(websocketpp::http::status_code::not_found, "symbol not found");
		return;
	}

	std::string body = con->get_request().get_body();

	std::cout << body << std::endl;

	try {
		Price price;
		price.fromJson(body);
		price.m_symbol = symbolIt->first;
		price.m_symbolName = symbolIt->second.m_name;
		m_prices[price.m_symbol] = price;
		con->set_status(websocketpp::http::status_code::ok);
	}
	catch (std::exception& e) {
		con->set_status(websocketpp::http::status_code::internal_server_error, e.what());
	}
}
