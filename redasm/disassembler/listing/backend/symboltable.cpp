#include "symboltable.h"
#include <redasm/support/demangler.h>
#include <redasm/support/utils.h>
#include <impl/disassembler/listing/backend/symboltable_impl.h>
#include <impl/libs/cereal/archives/binary.hpp>
#include <forward_list>
#include <sstream>

namespace REDasm {

Symbol::Symbol(): type(SymbolType::None), flags(SymbolFlags::None), tag(0), address(0) { }
Symbol::Symbol(SymbolType type, SymbolFlags flags, tag_t tag, address_t address, const String& name): type(type), flags(flags), tag(tag), address(address), name(name) { }
void Symbol::lock() { type |= SymbolType::Locked; }
bool Symbol::is(SymbolType t) const { return type == t; }
bool Symbol::typeIs(SymbolType t) const { return type == t; }
bool Symbol::hasFlag(SymbolFlags flag) const { return flags & flag; }
bool Symbol::isFunction() const { return type & SymbolType::FunctionMask; }
bool Symbol::isImport() const { return type & SymbolType::ImportMask; }
bool Symbol::isExport() const { return type & SymbolType::ExportMask; }
bool Symbol::isEntryPoint() const { return type & SymbolType::EntryPointMask; }
bool Symbol::isLocked() const { return type & SymbolType::Locked; }

bool Symbol::isData() const
{
    switch(this->type)
    {
        case SymbolType::DataNew:
        case SymbolType::StringNew: return true;
        default: break;
    }

    return false;
}

bool Symbol::isCode() const
{
    switch(this->type)
    {
        case SymbolType::LabelNew:
        case SymbolType::FunctionNew: return true;
        default: break;
    }

    return false;
}

void Symbol::save(cereal::BinaryOutputArchive &a) const { a(type, tag, address, name); }
void Symbol::load(cereal::BinaryInputArchive &a) { a(type, tag, address, name); }

SymbolTable::SymbolTable(): m_pimpl_p(new SymbolTableImpl()) { }
size_t SymbolTable::size() const { PIMPL_P(const SymbolTable); return p->m_byaddress.size(); }

bool SymbolTable::create(address_t address, const String &name, SymbolType type, SymbolFlags flags, tag_t tag)
{
    PIMPL_P(SymbolTable);
    auto it = p->m_byaddress.find(address);

    if(it != p->m_byaddress.end())
    {
        const SymbolPtr& symbol = it->second;
        if(symbol->type > type) return false;
    }

    p->m_byaddress.emplace(address, std::make_unique<Symbol>(type, flags, tag, address, name));
    p->m_byname[name] = address;
    return it == p->m_byaddress.end();
}

bool SymbolTable::create(address_t address, const String& name, SymbolType type, tag_t tag) { return this->create(address, name, type, SymbolFlags::None, tag); }
bool SymbolTable::create(address_t address, SymbolType type, SymbolFlags flags, tag_t tag) { return this->create(address, SymbolTable::name(address, type, flags), type, flags, tag); }
bool SymbolTable::create(address_t address, SymbolType type, tag_t tag) { return this->create(address, SymbolTable::name(address, type), type, SymbolFlags::None, tag); }

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

void SymbolTable::clear() { PIMPL_P(SymbolTable); p->m_byaddress.clear(); p->m_byname.clear(); }
void SymbolTable::save(cereal::BinaryOutputArchive &a) const { PIMPL_P(const SymbolTable); p->save(a); }
void SymbolTable::load(cereal::BinaryInputArchive &a) { PIMPL_P(SymbolTable); p->load(a); }

String SymbolTable::normalized(const String& s)
{
    if(Demangler::isMangled(s)) return Demangler::demangled(s);
    return String(s).replace(' ', '_');
}

String SymbolTable::name(address_t address, SymbolType type, SymbolFlags flags)
{
    std::stringstream ss;
    ss << SymbolTableImpl::prefix(type, flags).c_str() << "_" << std::hex << address;
    return ss.str().c_str();
}

String SymbolTable::name(address_t address, const String &s, SymbolType type, SymbolFlags flags)
{
    if(s.empty())
        return SymbolTable::name(address, type, flags);

    std::stringstream ss;
    ss << SymbolTableImpl::prefix(type, flags).c_str() << "_" << s.c_str() << "_" << std::hex << address;
    return ss.str().c_str();
}

String SymbolTable::name(const String &name, address_t address) { return name + "_"  + String::hex(address); }

}
