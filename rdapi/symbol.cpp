#include "symbol.h"
#include <rdcore/document/backend/symboltable.h>

const char* RDSymbol_NameHint(rd_address address, const char* s, rd_type type, rd_flag flags)
{
    static std::string res;
    res = SymbolTable::name(address, s, type, flags);
    return res.c_str();
}
