#include "symboltable_impl.h"
#include "../libs/cereal/archives/binary.hpp"

namespace REDasm {

void SymbolTableImpl::save(cereal::BinaryOutputArchive &a) const
{
    size_t size = m_byaddress.size();
    a(size);

    for(auto& item : m_byaddress)
    {
        a(item.first);
        item.second->save(a);
    }
}

void SymbolTableImpl::load(cereal::BinaryInputArchive &a)
{
    size_t size = 0;
    a(size);

    for(size_t i = 0; i < size; i++)
    {
        address_t address;
        auto symbol = std::make_unique<Symbol>();
        a(address);
        symbol->load(a);

        m_byname[symbol->name] = address;
        m_byaddress[symbol->address] = std::move(symbol);
    }
}

String SymbolTableImpl::prefix(SymbolType type)
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

} // namespace REDasm
