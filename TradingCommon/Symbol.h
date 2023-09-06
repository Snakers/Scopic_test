#pragma once

#include <string>

#include "json.h"

class Symbol
{
public:
	Symbol();
	Symbol(const std::string& id, const std::string& name)
		:m_id(id), m_name(name)
	{}
	std::string toJson() const;
	json::JSON toJsonObject() const;
	void fromJson(const std::string& jsonStr);

	std::string m_id;
	std::string m_name;
};