#include "Symbol.h"

Symbol::Symbol()
{
}

std::string Symbol::toJson() const
{
    return toJsonObject().dump();
}

json::JSON Symbol::toJsonObject() const
{
    json::JSON obj = json::Object();
    obj["id"] = m_id;
    obj["symbolName"] = m_name;
    return obj;
}

void Symbol::fromJson(const std::string& jsonStr)
{
    json::JSON obj = json::JSON::Load(jsonStr);
    if (obj.IsNull()) {
        throw "can not parse json";
    }
    m_id = obj["id"].ToString();
    m_name = obj["symbolName"].ToString();
}
