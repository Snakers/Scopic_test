#ifndef PRICE_H
#define PRICE_H

#include <string>

#include "json.h"

class Price
{
public:
	Price();
	Price(const std::string& symbol, const std::string& symbolName, double price, int quantity, uint64_t timestamp);

	std::string toJson() const;
	json::JSON toJsonObject() const;
	void fromJson(const std::string& jsonStr);

	std::string m_symbol;
	std::string m_symbolName;
	double m_price;
	int m_quantity;
	uint64_t m_timestamp;
};

#endif // !PRICE_H