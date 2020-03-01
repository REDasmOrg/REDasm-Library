#pragma once

#include "../../../../types/api.h"

namespace REDasm {

DECLARE_TYPES(Symbol, None, Data, String, Label, Function, Import)

namespace SymbolFlags {
enum: flag_t {
     None         = 0,
     Weak         = (1 << 0),
     Export       = (1 << 1),
     EntryPoint   = (1 << 2),
     AsciiString  = (1 << 3),
     WideString   = (1 << 4),
     Pointer      = (1 << 5),
     TableItem    = (1 << 6),
};
} // namespace SymbolFlags

struct Symbol
{
    type_t type;
    flag_t flags;
    tag_t tag{0};
    address_t address;
    String name;

    Symbol() = default;
    Symbol(type_t type, flag_t flags, tag_t tag, address_t address, const String& name);
    bool isFunction() const;
    bool isImport() const;
    bool isExport() const;
    bool isString() const;
    bool isAsciiString() const;
    bool isWideString() const;
    bool isEntryPoint() const;
    bool isTableItem() const;
    bool isPointer() const;
    bool isLabel() const;
    bool isData() const;
    bool isCode() const;
    bool isWeak() const;
};

} // namespace REDasm
