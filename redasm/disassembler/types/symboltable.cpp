#include "symboltable.h"
#include <forward_list>

namespace REDasm {

bool SymbolTable::create(address_t address, const std::string &name, u32 type, u32 tag)
{
    auto it = m_byaddress.find(address);

    if(it != m_byaddress.end())
    {
        const SymbolPtr& symbol = it->second;

        if(symbol->isLocked())
            return false;
    }

    m_byaddress.emplace(address, std::make_unique<Symbol>(type, tag, address, name));
    m_byname[name] = address;
    return it == m_byaddress.end();
}

Symbol* SymbolTable::symbol(const std::string &name) const
{
    auto it = m_byname.find(name);

    if(it != m_byname.end())
        return m_byaddress.at(it->second).get();

    return nullptr;
}

Symbol* SymbolTable::symbol(address_t address) const
{
    auto it = m_byaddress.find(address);

    if(it == m_byaddress.end())
        return nullptr;

    return it->second.get();
}

void SymbolTable::iterate(u32 symbolflags, const std::function<bool(const Symbol*)>& cb) const
{
    std::forward_list<const Symbol*> symbols;

    for(auto it = m_byaddress.begin(); it != m_byaddress.end(); it++)
    {
        const SymbolPtr& symbol = it->second;

        if(!((symbol->type & SymbolTypes::LockedMask) & symbolflags))
            continue;

        symbols.emplace_front(symbol.get());
    }

    for(auto it = symbols.begin(); it != symbols.end(); it++)
    {
        if(!cb(*it))
            break;
    }
}

bool SymbolTable::erase(address_t address)
{
    auto it = m_byaddress.find(address);

    if(it == m_byaddress.end())
        return false;

    const SymbolPtr& symbol = it->second;

    if(!symbol)
        return false;

    m_byname.erase(symbol->name);
    m_byaddress.erase(it);
    return true;
}

void SymbolTable::clear()
{
    m_byaddress.clear();
    m_byname.clear();
}

std::string SymbolTable::name(address_t address, u32 type)
{
    std::stringstream ss;
    ss << SymbolTable::prefix(type) << "_" << std::hex << address;
    return ss.str();
}

std::string SymbolTable::name(address_t address, const std::string &s, u32 type)
{
    if(s.empty())
        return SymbolTable::name(address, type);

    std::stringstream ss;
    ss << SymbolTable::prefix(type) << "_" << s << "_" << std::hex << address;
    return ss.str();
}

void SymbolTable::serializeTo(std::fstream &fs)
{
    Serializer::serializeMap<address_t, SymbolPtr>(fs, m_byaddress, [&](address_t k, const SymbolPtr& v) {
        Serializer::serializeScalar(fs, k);
        this->serializeSymbol(fs, v);
    });
}

void SymbolTable::deserializeFrom(std::fstream &fs)
{
    EVENT_CONNECT(this, deserialized, this, std::bind(&SymbolTable::bindName, this, std::placeholders::_1));

    Serializer::deserializeMap<address_t, SymbolPtr>(fs, m_byaddress, [&](address_t& k, SymbolPtr& v) {
        Serializer::deserializeScalar(fs, &k);
        this->deserializeSymbol(fs, v);
        deserialized(v.get());
    });

    this->deserialized.removeLast();
}

std::string SymbolTable::prefix(u32 type)
{
    if(type & SymbolTypes::Pointer)
        return "ptr";
    if(type & SymbolTypes::WideStringMask)
        return "wstr";
    if(type & SymbolTypes::StringMask)
        return "str";
    else if(type & SymbolTypes::FunctionMask)
        return "sub";
    else if(type & SymbolTypes::Code)
        return "loc";

    return "data";
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
    value = std::make_unique<Symbol>();
    Serializer::deserializeScalar(fs, &value->type);
    Serializer::deserializeScalar(fs, &value->tag);
    Serializer::deserializeScalar(fs, &value->address);
    Serializer::deserializeScalar(fs, &value->size);
    Serializer::deserializeString(fs, value->name);
}

void SymbolTable::bindName(const Symbol* symbol) { m_byname[symbol->name] = symbol->address; }

}
