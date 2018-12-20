#include "symboltable.h"

namespace REDasm {

SymbolTable::SymbolTable() { }

bool SymbolTable::create(address_t address, const std::string &name, u32 type, u32 tag)
{
    auto it = m_byaddress.find(address);

    if(it != m_byaddress.end())
    {
        SymbolPtr symbol = it->second;

        if(symbol->isLocked())
            return false;
    }

    m_byaddress.emplace(address, std::make_shared<Symbol>(type, tag, address, name));
    m_byname[name] = address;
    return it == m_byaddress.end();
}

SymbolPtr SymbolTable::symbol(const std::string &name)
{
    auto it = m_byname.find(name);

    if(it != m_byname.end())
        return m_byaddress[it->second];

    return NULL;
}

SymbolPtr SymbolTable::symbol(address_t address)
{
    auto it = m_byaddress.find(address);

    if(it == m_byaddress.end())
        return NULL;

    return it->second;
}

void SymbolTable::iterate(u32 symbolflags, std::function<bool (const SymbolPtr&)> f)
{
    std::list<SymbolPtr> symbols;

    for(auto it = m_byaddress.begin(); it != m_byaddress.end(); it++)
    {
        SymbolPtr symbol = it->second;

        if(!((symbol->type & SymbolTypes::LockedMask) & symbolflags))
            continue;

        symbols.push_back(symbol);
    }

    for(auto it = symbols.begin(); it != symbols.end(); it++)
    {
        if(!f(*it))
            break;
    }
}

bool SymbolTable::erase(address_t address)
{
    auto it = m_byaddress.find(address);

    if(it == m_byaddress.end())
        return false;

    SymbolPtr symbol = it->second;

    if(!symbol)
        return false;

    m_byaddress.erase(it);
    m_byname.erase(symbol->name);
    return true;
}

void SymbolTable::serializeTo(std::fstream &fs)
{
    Serializer::serializeMap<address_t, SymbolPtr>(fs, m_byaddress, [&](const std::pair<address_t, SymbolPtr>& item) {
        Serializer::serializeScalar(fs, item.first);
        this->serializeSymbol(fs, item.second);
    });
}

void SymbolTable::deserializeFrom(std::fstream &fs)
{
    this->deserialized += std::bind(&SymbolTable::bindName, this, std::placeholders::_1);

    Serializer::deserializeMap<address_t, SymbolPtr>(fs, m_byaddress, [&](std::pair<address_t, SymbolPtr>& item) {
        Serializer::deserializeScalar(fs, &item.first);
        this->deserializeSymbol(fs, item.second);
        deserialized(item.second);
    });

    this->deserialized.removeLast();
}

void SymbolTable::serializeSymbol(std::fstream &fs, const SymbolPtr &value)
{
    Serializer::serializeScalar(fs, value->type);
    Serializer::serializeScalar(fs, value->tag);
    Serializer::serializeScalar(fs, value->address);
    Serializer::serializeScalar(fs, value->size);
    Serializer::serializeString(fs, value->name);
}

void SymbolTable::deserializeSymbol(std::fstream &fs, SymbolPtr &value)
{
    value = std::make_shared<Symbol>();
    Serializer::deserializeScalar(fs, &value->type);
    Serializer::deserializeScalar(fs, &value->tag);
    Serializer::deserializeScalar(fs, &value->address);
    Serializer::deserializeScalar(fs, &value->size);
    Serializer::deserializeString(fs, value->name);
}

void SymbolTable::bindName(const SymbolPtr &symbol) { m_byname[symbol->name] = symbol->address; }

}
