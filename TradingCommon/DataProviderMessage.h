#ifndef DATAPROVIDERMESSAGE_H
#define DATAPROVIDERMESSAGE_H

#include <string>

#include "json.h"

struct DataProviderMessage
{
    DataProviderMessage()
        : m_price(0.0)
        , m_quantity(0)
        , m_timestamp(0)
    {}
    DataProviderMessage(const std::string& symbol, double price, int quantity, int timestampDifference);
    DataProviderMessage(const std::string& symbol ,const std::string& name, double price, int quantity, int timestampDifference);

    void updateTimestamp();

    std::string toJson() const;
    json::JSON toJsonObject() const;
    void fromJson(const std::string& jsonStr);

    std::string m_symbol;
    std::string m_symbolName;
    double m_price;
    int m_quantity;
    uint64_t m_timestamp;
    int m_timestampDifference;
};

#endif // DATAPROVIDERMESSAGE_H
