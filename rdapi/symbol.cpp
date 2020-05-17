#include "symbol.h"
#include <rdcore/document/backend/symboltable.h>

const char* RDSymbol_NameHint(address_t address, const char* s, type_t type, flag_t flags)
{
    static std::string res;
    res = SymbolTable::name(address, s, type, flags);
    return res.c_str();
}
