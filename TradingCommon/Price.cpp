#include "Price.h"
#include "json.h"

Price::Price()
	: m_price(0.0)
	, m_quantity(0)
	, m_timestamp(0)
{
}

Price::Price(const std::string& symbol, const std::string& symbolName, double price, int quantity, uint64_t timestamp)
	: m_symbol(symbol)
	, m_symbolName(symbolName)
	, m_price(price)
	, m_quantity(quantity)
	, m_timestamp(timestamp)
{
}

std::string Price::toJson() const
{
	std::string str = toJsonObject().dump();
	return str;
}

json::JSON Price::toJsonObject() const
{
	json::JSON obj = json::Object();
	obj["symbol"] = m_symbol;
	obj["symbolName"] = m_symbolName;
	obj["price"] = m_price;
	obj["quantity"] = m_quantity;
	obj["timestamp"] = m_timestamp;
	return obj;
}

void Price::fromJson(const std::string& jsonStr)
{
	json::JSON obj = json::JSON::Load(jsonStr);
	if (obj.IsNull()) {
		throw "can not parse json";
	}
	m_symbol = obj["symbol"].ToString();
	m_symbolName = obj["symbolName"].ToString();
	m_quantity = obj["quantity"].ToInt();
	m_timestamp = obj["timestamp"].ToInt();

	auto priceObj = obj["price"];
	m_price = priceObj.JSONType() == json::JSON::Class::Floating ? priceObj.ToFloat() : priceObj.ToInt();
}