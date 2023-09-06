#include "DataProviderMessage.h"
#include "json.h"

#include <chrono>

DataProviderMessage::DataProviderMessage(const std::string& symbol, double price, int quantity, int timestampDifference)
    : m_symbol(symbol)
    , m_price(price)
    , m_quantity(quantity)
    , m_timestampDifference(timestampDifference)
{
    updateTimestamp();
}

DataProviderMessage::DataProviderMessage(const std::string &symbol, const std::string &symbolName, double price, int quantity, int timestampDifference)
    : m_symbol(symbol)
    , m_symbolName(symbolName)
    , m_price(price)
    , m_quantity(quantity)
    , m_timestampDifference(timestampDifference)
{

}

void DataProviderMessage::updateTimestamp()
{
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    m_timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() + m_timestampDifference;
}

std::string DataProviderMessage::toJson() const
{
    return toJsonObject().dump();
}

json::JSON DataProviderMessage::toJsonObject() const
{
    json::JSON obj = json::Object();
    obj["symbol"] = m_symbol;

    if(!m_symbolName.empty())
        obj["symbolName"] = m_symbolName;

    obj["price"] = m_price;
    obj["quantity"] = m_quantity;
    obj["timestamp"] = m_timestamp;

    return obj;
}

void DataProviderMessage::fromJson(const std::string& jsonStr)
{
    json::JSON obj = json::JSON::Load(jsonStr);
    if (obj.IsNull()) {
        throw "can not parse json";
    }
    m_symbol = obj["symbol"].ToString();

    if(!obj["name"].ToString().empty())
        m_symbolName = obj["symbolName"].ToString();

    m_quantity = obj["quantity"].ToInt();
    m_timestamp = obj["timestamp"].ToInt();

    auto priceObj = obj["price"];
    m_price = priceObj.JSONType() == json::JSON::Class::Floating ? priceObj.ToFloat() : priceObj.ToInt();
}
