#include "symboltable.h"
#include <redasm/support/demangler.h>
#include <redasm/support/utils.h>
#include <impl/disassembler/listing/backend/symbols/symboltable_impl.h>
#include <impl/libs/cereal/archives/binary.hpp>
#include <forward_list>
#include <sstream>

namespace REDasm {

SymbolTable::SymbolTable(): m_pimpl_p(new SymbolTableImpl()) { }
size_t SymbolTable::size() const { PIMPL_P(const SymbolTable); return p->m_byaddress.size(); }

void SymbolTable::create(address_t address, const String &name, type_t type, flag_t flags, tag_t tag)
{
    PIMPL_P(SymbolTable);
    const Symbol* symbol = this->get(address);
    if(symbol) p->m_byname.erase(symbol->name); // Remove old name reference

    p->m_byaddress[address] = std::make_unique<Symbol>(type, flags, tag, address, name);
    p->m_byname[name] = address;
}

void SymbolTable::create(address_t address, const String& name, type_t type, tag_t tag) { this->create(address, name, type, Symbol::T_None, tag); }
void SymbolTable::create(address_t address, type_t type, flag_t flags, tag_t tag) { this->create(address, SymbolTable::name(address, type, flags), type, flags, tag); }
void SymbolTable::create(address_t address, type_t type, tag_t tag) { this->create(address, SymbolTable::name(address, type), type, Symbol::T_None, tag); }

Symbol* SymbolTable::get(const String &name) const
{
    PIMPL_P(const SymbolTable);
    auto it = p->m_byname.find(name);

    if(it != p->m_byname.end())
        return p->m_byaddress.at(it->second).get();

    return nullptr;
}

Symbol* SymbolTable::get(address_t address) const
{
    PIMPL_P(const SymbolTable);
    auto it = p->m_byaddress.find(address);

    if(it == p->m_byaddress.end())
        return nullptr;

    return it->second.get();
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

void SymbolTable::clear() { PIMPL_P(SymbolTable); p->m_byaddress.clear(); p->m_byname.clear(); }
bool SymbolTable::rename(address_t address, const String& newname) { PIMPL_P(SymbolTable); return p->rename(address, newname); }
void SymbolTable::save(cereal::BinaryOutputArchive &a) const { PIMPL_P(const SymbolTable); p->save(a); }
void SymbolTable::load(cereal::BinaryInputArchive &a) { PIMPL_P(SymbolTable); p->load(a); }

String SymbolTable::normalized(const String& s)
{
    if(Demangler::isMangled(s)) return Demangler::demangled(s);
    return String(s).replace(' ', '_');
}

String SymbolTable::name(address_t address, type_t type, flag_t flags)
{
    std::stringstream ss;
    ss << SymbolTableImpl::prefix(type, flags).c_str() << "_" << std::hex << address;
    return ss.str().c_str();
}

String SymbolTable::name(address_t address, const String &s, type_t type, flag_t flags)
{
    if(s.empty())
        return SymbolTable::name(address, type, flags);

    std::stringstream ss;
    ss << SymbolTableImpl::prefix(type, flags).c_str() << "_" << s.c_str() << "_" << std::hex << address;
    return ss.str().c_str();
}

String SymbolTable::name(const String &name, address_t address) { return name + "_"  + String::hex(address); }

}
