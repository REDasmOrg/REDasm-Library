#pragma once

#include <redasm/types/api.h>

namespace REDasm {

enum class SymbolType: size_t {
    None = 0,
    Data,
    String,
    Label,
    Function,
    Import,
};

enum class SymbolFlags: size_t
{
    None         = 0,
    Weak         = (1 << 0),
    Export       = (1 << 1),
    EntryPoint   = (1 << 2),
    AsciiString  = (1 << 3),
    WideString   = (1 << 4),
    Pointer      = (1 << 5),
    TableItem    = (1 << 6),
};

ENUM_FLAGS_OPERATORS(SymbolFlags)

struct Symbol
{
    SymbolType type;
    SymbolFlags flags;
    tag_t tag{0};
    address_t address;
    String name;

    Symbol() = default;
    Symbol(SymbolType type, SymbolFlags flags, tag_t tag, address_t address, const String& name);
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
