#include "symboltable_impl.h"

namespace REDasm {

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
