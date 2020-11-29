#include "symboltable.h"
#include <cstring>
#include <sstream>
#include <iomanip>
#include <regex>
#include "../../context.h"
#include "../../support/utils.h"
#include "../../support/demangler.h"

SymbolTable::SymbolTable(Context* ctx): Object(ctx) { }

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

void SymbolTable::remove(rd_address address)
{
    auto it = m_byaddress.find(address);
    if(it == m_byaddress.end()) return;

    auto sit = m_stringtable.find(address);

    if(sit != m_stringtable.end())
    {
        m_byname.erase(sit->second);
        m_stringtable.erase(sit);
    }

    m_bytype[it->second.type].erase(address);
    m_byaddress.erase(address);
}

void SymbolTable::each(const SymbolTable::AddressCallback& cb) const
{
    for(const auto& [address, symbol] : m_byaddress)
    {
        if(!cb(address)) break;
    }
}

void SymbolTable::eachType(rd_type type, const SymbolTable::AddressCallback& cb) const
{
    auto it = m_bytype.find(type);
    if(it == m_bytype.end()) return;

    for(rd_address address : it->second)
    {
        if(!cb(address)) break;
    }
}

bool SymbolTable::find(const std::string& q, RDSymbol* symbol, rd_type hint) const { return this->findR(Utils::wildcardToRegex(q), symbol, hint); }
size_t SymbolTable::findAll(const std::string& q, const RDSymbol** symbols, rd_type hint) const { return this->findAllR(Utils::wildcardToRegex(q), symbols, hint); }

bool SymbolTable::findR(const std::string& q, RDSymbol* symbol, rd_type hint) const
{
    if(hint != SymbolType_None) return this->findByType(q, symbol, hint);

    for(const auto& [a, s] : m_byaddress)
    {
        if(!this->isSymbolAccepted(q, a)) continue;
        if(symbol) *symbol = s;
        return true;
    }

    return false;
}

size_t SymbolTable::findAllR(const std::string& q, const RDSymbol** symbols, rd_type hint) const
{
    m_result.clear();
    if(hint != SymbolType_None) return this->findAllByType(q, symbols, hint);

    for(const auto& [a, s] : m_byaddress)
    {
        if(!this->isSymbolAccepted(q, a)) continue;
        m_result.push_back(s);
    }

    if(symbols) *symbols = m_result.data();
    return m_result.size();
}

SymbolTable::ByAddress::const_iterator SymbolTable::begin() const { return m_byaddress.begin(); }
SymbolTable::ByAddress::const_iterator SymbolTable::end() const { return m_byaddress.end(); }

void SymbolTable::create(rd_address address, std::string name, rd_type type, rd_flag flags)
{
    if(name.empty()) name = SymbolTable::name(address, type, flags);
    else if(!this->context()->hasFlag(ContextFlags_NoDemangle)) name = Demangler::demangled(name);

    m_byaddress[address] = { address, type, flags };
    m_byname[name] = address;
    m_stringtable[address] = name;
    m_bytype[type].insert(address);
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

bool SymbolTable::isSymbolAccepted(const std::string& q, rd_address address) const
{
    auto it = m_stringtable.find(address);
    if(it == m_stringtable.end()) return false;

    std::regex r(q);
    return std::regex_search(it->second, r);
}

bool SymbolTable::findByType(const std::string& q, RDSymbol* symbol, rd_type type) const
{
    auto it = m_bytype.find(type);
    if(it == m_bytype.end()) return false;

    for(rd_address address : it->second)
    {
        if(!this->isSymbolAccepted(q, address)) continue;

        auto it = m_byaddress.find(address);
        if(it == m_byaddress.end()) return false;

        if(symbol) *symbol = it->second;
        return true;
    }

    return false;
}

size_t SymbolTable::findAllByType(const std::string& q, const RDSymbol** symbols, rd_type type) const
{
    auto it = m_bytype.find(type);
    if(it == m_bytype.end()) return false;

    for(rd_address address : it->second)
    {
        if(!this->isSymbolAccepted(q, address)) continue;

        auto it = m_byaddress.find(address);
        if(it == m_byaddress.end()) continue;

        m_result.push_back(it->second);
    }

    if(symbols) *symbols = m_result.data();
    return m_result.size();
}

std::string SymbolTable::name(rd_address address, const char* s, rd_type type, rd_flag flags)
{
    if(!s || !std::strlen(s))
        return SymbolTable::name(address, type, flags);

    std::stringstream ss;
    ss << SymbolTable::prefix(type, flags) << "_" << s << "_" << std::hex << address;
    return ss.str();
}

std::string SymbolTable::name(const char* s, rd_address address) { return std::string(s) + "_" + Utils::hex(address); }

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
