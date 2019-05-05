#include "symboltable.h"
#include <redasm/support/demangler.h>
#include <forward_list>

namespace REDasm {

bool SymbolTable::create(address_t address, const std::string &name, SymbolType type, tag_t tag)
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

void SymbolTable::iterate(SymbolType type, const std::function<bool(const Symbol*)>& cb) const
{
    std::forward_list<const Symbol*> symbols;

    for(auto it = m_byaddress.begin(); it != m_byaddress.end(); it++)
    {
        const SymbolPtr& symbol = it->second;

        if(!(symbol->type & type))
            continue;

        symbols.push_front(symbol.get()); // m_byaddress can be modified
    }

    for(const Symbol* symbol : symbols)
        cb(symbol);
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

std::string SymbolTable::normalized(std::string s)
{
    if(Demangler::isMangled(s))
        return Demangler::demangled(s);

    std::replace(s.begin(), s.end(), ' ', '_');
    return s;
}

std::string SymbolTable::name(address_t address, SymbolType type)
{
    std::stringstream ss;
    ss << SymbolTable::prefix(type) << "_" << std::hex << address;
    return ss.str();
}

std::string SymbolTable::name(address_t address, const std::string &s, SymbolType type)
{
    if(s.empty())
        return SymbolTable::name(address, type);

    std::stringstream ss;
    ss << SymbolTable::prefix(type) << "_" << s << "_" << std::hex << address;
    return ss.str();
}

std::string SymbolTable::prefix(SymbolType type)
{
    if(type & SymbolType::Pointer)
        return "ptr";
    if(type & SymbolType::WideStringMask)
        return "wstr";
    if(type & SymbolType::StringMask)
        return "str";
    if(type & SymbolType::FunctionMask)
        return "sub";
    if(type & SymbolType::Code)
        return "loc";
    if(type & SymbolType::TableItemMask)
        return "tbl";

    return "data";
}

}
