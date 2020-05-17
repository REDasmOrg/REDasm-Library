#include "symboltable.h"
#include <cstring>
#include <sstream>
#include <iomanip>
#include "../../support/utils.h"

size_t SymbolTable::size() const { return m_byaddress.size(); }

const char* SymbolTable::getName(address_t address) const
{
    auto it = m_stringtable.find(address);
    return (it != m_stringtable.end()) ? it->second.c_str() : nullptr;
}

bool SymbolTable::get(address_t address, RDSymbol* symbol) const
{
    if(!symbol) return false;

    auto it = m_byaddress.find(address);
    if(it == m_byaddress.end()) return false;

    *symbol = it->second;
    return true;
}

bool SymbolTable::get(const char* name, RDSymbol* symbol) const
{
    auto it = m_byname.find(name);
    if(it == m_byname.end()) return false;
    return this->get(it->second, symbol);
}

void SymbolTable::remove(address_t address)
{
    auto it = m_stringtable.find(address);
    if(it == m_stringtable.end()) return;

    m_byname.erase(it->second);
    m_stringtable.erase(it);
    m_byaddress.erase(address);
}

void SymbolTable::create(address_t address, const std::string& name, type_t type, flag_t flags)
{
    std::string symbolname = name;
    if(symbolname.empty()) symbolname = SymbolTable::name(address, type, flags);

    m_byaddress[address] = { address, type, flags };
    m_byname[symbolname] = address;
    m_stringtable[address] = symbolname;
}

bool SymbolTable::rename(address_t address, const std::string& newname)
{
    auto it = m_byaddress.find(address);
    if(it == m_byaddress.end()) return false;

    m_byname[newname] = address;
    m_stringtable[address] = newname;
    return true;
}

std::string SymbolTable::normalized(const char* s)
{
    return s;
}

std::string SymbolTable::name(address_t address, type_t type, flag_t flags)
{
    std::stringstream ss;
    ss << SymbolTable::prefix(type, flags).c_str() << "_" << std::hex << address;
    return ss.str();
}

std::string SymbolTable::name(address_t address, const char* s, type_t type, flag_t flags)
{
    if(!s || !std::strlen(s))
        return SymbolTable::name(address, type, flags);

    std::stringstream ss;
    ss << SymbolTable::prefix(type, flags) << "_" << s << "_" << std::hex << address;
    return ss.str();
}

std::string SymbolTable::name(const char* s, address_t address)
{
    return std::string(s) + "_" + Utils::hex(address);
}

std::string SymbolTable::prefix(type_t type, flag_t flags)
{
    switch(type)
    {
        case SymbolType_String:   return (flags & SymbolFlags_WideString) ? "wstr" : "str";
        case SymbolType_Label:    return "loc";
        case SymbolType_Function: return "sub";
        case SymbolType_Import:   return "imp";
        default: break;
    }

    if(flags & SymbolFlags_TableItem) return "tbl";
    if(flags & SymbolFlags_Pointer) return "ptr";

    return "data";
}
