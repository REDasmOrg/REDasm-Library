#pragma once

#include "types.h"
#include "macros.h"

enum RDSymbolType {
    SymbolType_None,
    SymbolType_Data,
    SymbolType_String,
    SymbolType_Label,
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
    SymbolFlags_TableItem   = (1 << 6),
};

typedef struct RDSymbol {
    address_t address;
    type_t type;
    flag_t flags;
} RDSymbol;

