#pragma once

#include "../../../../types/api.h"

namespace REDasm {

struct Symbol
{
    enum Type: type_t {
        T_None, T_Data, T_String, T_Label, T_Function, T_Import
    };

    enum Flags: flag_t {
         F_Weak         = (1 << 0),
         F_Export       = (1 << 1),
         F_EntryPoint   = (1 << 2),
         F_AsciiString  = (1 << 3),
         F_WideString   = (1 << 4),
         F_Pointer      = (1 << 5),
         F_TableItem    = (1 << 6),
    };

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
