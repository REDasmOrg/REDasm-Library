#include "symboltable.h"
#include <redasm/support/demangler.h>
#include <redasm/support/utils.h>
#include <impl/disassembler/types/symboltable_impl.h>
#include <forward_list>
#include <sstream>

namespace REDasm {

SymbolTable::SymbolTable(): m_pimpl_p(new SymbolTableImpl()) { }

bool SymbolTable::create(address_t address, const String &name, SymbolType type, tag_t tag)
{
    PIMPL_P(SymbolTable);
    auto it = p->m_byaddress.find(address);

    if(it != p->m_byaddress.end())
    {
        const SymbolPtr& symbol = it->second;

        if(symbol->isLocked())
            return false;
    }

    p->m_byaddress.emplace(address, std::make_unique<Symbol>(type, tag, address, name));
    p->m_byname[name] = address;
    return it == p->m_byaddress.end();
}

Symbol* SymbolTable::symbol(const String &name) const
{
    PIMPL_P(const SymbolTable);
    auto it = p->m_byname.find(name);

    if(it != p->m_byname.end())
        return p->m_byaddress.at(it->second).get();

    return nullptr;
}

Symbol* SymbolTable::symbol(address_t address) const
{
    PIMPL_P(const SymbolTable);
    auto it = p->m_byaddress.find(address);

    if(it == p->m_byaddress.end())
        return nullptr;

    return it->second.get();
}

void SymbolTable::iterate(SymbolType type, const std::function<bool(const Symbol*)>& cb) const
{
    PIMPL_P(const SymbolTable);
    std::forward_list<const Symbol*> symbols;

    for(auto it = p->m_byaddress.begin(); it != p->m_byaddress.end(); it++)
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
    PIMPL_P(SymbolTable);
    auto it = p->m_byaddress.find(address);

    if(it == p->m_byaddress.end())
        return false;

    const SymbolPtr& symbol = it->second;

    if(!symbol)
        return false;

    p->m_byname.erase(symbol->name);
    p->m_byaddress.erase(it);
    return true;
}

void SymbolTable::clear()
{
    PIMPL_P(SymbolTable);
    p->m_byaddress.clear();
    p->m_byname.clear();
}

String SymbolTable::normalized(const String& s)
{
    if(Demangler::isMangled(s))
        return Demangler::demangled(s);

    return String(s).replace(' ', '_');
}

String SymbolTable::name(address_t address, SymbolType type)
{
    std::stringstream ss;
    ss << SymbolTableImpl::prefix(type).c_str() << "_" << std::hex << address;
    return ss.str().c_str();
}

String SymbolTable::name(address_t address, const String &s, SymbolType type)
{
    if(s.empty())
        return SymbolTable::name(address, type);

    std::stringstream ss;
    ss << SymbolTableImpl::prefix(type).c_str() << "_" << s.c_str() << "_" << std::hex << address;
    return ss.str().c_str();
}

String SymbolTable::name(const String &name, address_t address) { return name + "_"  + String::hex(address); }

void Serializer<SymbolTable>::write(std::fstream& fs, const SymbolTable* st) {
    Serializer<SymbolTableImpl::SymbolsByAddress>::write(fs, st->pimpl_p()->m_byaddress);
}

void Serializer<SymbolTable>::read(std::fstream& fs, SymbolTable* st) {
    Serializer<SymbolTableImpl::SymbolsByAddress>::read(fs, [st](address_t k, SymbolPtr v) {
        st->pimpl_p()->m_byname[v->name] = k;
        st->pimpl_p()->m_byaddress[k] = std::move(v);
    });
}

}
