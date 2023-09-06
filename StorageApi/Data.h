#pragma once

#include "Symbol.h"

#include <map>

std::pair<std::string, Symbol> createSymbolMapObject(const Symbol& symbol)
{
    return std::make_pair(symbol.m_id, symbol);
}

std::map<decltype(Symbol::m_id), Symbol> symbols = {
    createSymbolMapObject(Symbol("a631dc6c-ee85-458d-80d7-50018aedfbad", "TSLA")),
    createSymbolMapObject(Symbol("7f00cfc3-aa6c-414c-8d25-15997c2a0522", "PLTR")),
    createSymbolMapObject(Symbol("298932f7-74ea-4404-b544-3551289d8135", "NIO")),
    createSymbolMapObject(Symbol("531532e1-e27b-4aec-bf7d-92d5b20645af", "PFE")),
    createSymbolMapObject(Symbol("9e8bff74-50cd-4d80-900c-b5ce3bf371ee", "AMZN")),
    createSymbolMapObject(Symbol("629500dd-a8a7-474f-9444-e55340fb639e", "AMD")),
    createSymbolMapObject(Symbol("dd28e4b7-d20d-4d33-b76d-4f96440bfcaf", "AAPL")),
    createSymbolMapObject(Symbol("256c6786-5198-4d11-951b-3cea4e5e6af4", "NVDA")),
    createSymbolMapObject(Symbol("8805a3e0-9c32-4179-8171-7eebf3c240c4", "GOOGL")),
    createSymbolMapObject(Symbol("83e29024-0666-412d-b17e-e1b2745230ef", "BAC")),
    createSymbolMapObject(Symbol("b102925f-c37a-4adc-b89b-ba68e085b1e8", "AAL")),
    createSymbolMapObject(Symbol("257f9954-bd82-443d-8c6c-3ba81470f76c", "META")),
    createSymbolMapObject(Symbol("ce38cfe2-dd68-406d-b6e9-adf79b449c15", "UBER"))
};