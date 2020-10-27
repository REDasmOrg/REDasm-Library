#include "symboltable.h"
#include <cstring>
#include <sstream>
#include <iomanip>
#include "../../context.h"
#include "../../support/utils.h"
#include "../../support/demangler.h"

SymbolTable::SymbolTable(Context* ctx): Object(ctx) { }
size_t SymbolTable::size() const { return m_byaddress.size(); }

const char* SymbolTable::getName(rd_address address) const
{
    auto it = m_stringtable.find(address);
    return (it != m_stringtable.end()) ? it->second.c_str() : nullptr;
}

bool SymbolTable::get(rd_address address, RDSymbol* symbol) const
{
    auto it = m_byaddress.find(address);
    if(it == m_byaddress.end()) return false;

    if(symbol) *symbol = it->second;
    return true;
}

bool SymbolTable::get(const char* name, RDSymbol* symbol) const
{
    if(!this->context()->hasFlag(ContextFlags_NoDemangle))
        name = Demangler::demangled(name);

    auto it = m_byname.find(name);
    if(it == m_byname.end()) return false;
    return this->get(it->second, symbol);
}

bool SymbolTable::at(size_t idx, RDSymbol* symbol) const
{
    if(idx >= m_addresses.size()) return false;
    return this->get(m_addresses.at(idx), symbol);
}

void SymbolTable::remove(rd_address address)
{
    auto it = m_stringtable.find(address);
    if(it == m_stringtable.end()) return;

    m_addresses.remove(address);
    m_byname.erase(it->second);
    m_stringtable.erase(it);
    m_byaddress.erase(address);
}

void SymbolTable::create(rd_address address, std::string name, rd_type type, rd_flag flags)
{
    if(name.empty()) name = SymbolTable::name(address, type, flags);
    else if(!this->context()->hasFlag(ContextFlags_NoDemangle)) name = Demangler::demangled(name);

    m_addresses.insert(address);
    m_byaddress[address] = { address, type, flags };
    m_byname[name] = address;
    m_stringtable[address] = name;
}

bool SymbolTable::rename(rd_address address, std::string newname)
{
    if(newname.empty()) return false;

    if(!this->context()->hasFlag(ContextFlags_NoDemangle))
        newname = Demangler::demangled(newname);

    auto ait = m_byaddress.find(address);
    if(ait == m_byaddress.end()) return false;

    auto nit = m_byname.find(newname);

    if(nit != m_byname.end())
    {
        if(nit->second == address) return true; // Name is unchanged
        return false; // This name belongs to another symbol
    }

    m_byname[newname] = address;
    m_stringtable[address] = newname;
    return true;
}

std::string SymbolTable::name(rd_address address, rd_type type, rd_flag flags)
{
    std::stringstream ss;
    ss << SymbolTable::prefix(type, flags).c_str() << "_" << std::hex << address;
    return ss.str();
}

std::string SymbolTable::name(rd_address address, const char* s, rd_type type, rd_flag flags)
{
    if(!s || !std::strlen(s))
        return SymbolTable::name(address, type, flags);

    std::stringstream ss;
    ss << SymbolTable::prefix(type, flags) << "_" << s << "_" << std::hex << address;
    return ss.str();
}

std::string SymbolTable::name(const char* s, rd_address address)
{
    return std::string(s) + "_" + Utils::hex(address);
}

std::string SymbolTable::prefix(rd_type type, rd_flag flags)
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
