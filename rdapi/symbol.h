#pragma once

#include "types.h"
#include "macros.h"

enum RDSymbolType {
    SymbolType_None,
    SymbolType_Data,
    SymbolType_Location,
    SymbolType_String,
    SymbolType_Function,
    SymbolType_Import,
};

enum RDSymbolFlags {
    SymbolFlags_None        = 0,
    SymbolFlags_Weak        = (1 << 0),
    SymbolFlags_Export      = (1 << 1),
    SymbolFlags_EntryPoint  = (1 << 2),
    SymbolFlags_AsciiString = (1 << 3),
    SymbolFlags_WideString  = (1 << 4),
    SymbolFlags_Pointer     = (1 << 5),
    SymbolFlags_NoReturn    = (1 << 7),
    SymbolFlags_Field       = (1 << 8),
};

typedef struct RDSymbol {
    rd_address address;
    rd_type type;
    rd_flag flags;
} RDSymbol;

RD_API_EXPORT const char* RDSymbol_NameHint(rd_address address, const char* s, rd_type type, rd_flag flags);
