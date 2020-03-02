#pragma once

#include "../../../../types/base.h"
#include "../../../../types/string.h"
#include "../../../../macros.h"

namespace REDasm {

class LIBREDASM_API BlockItem: public Object
{
    public:
        enum Type: type_t { T_Unexplored, T_Data, T_Code };

        enum Flags: flag_t {
            F_None        = 0,
            F_Function    = (1 << 0),
            F_Label       = (1 << 1),
            F_AsciiString = (1 << 2),
            F_WideString  = (1 << 3),
            F_Import      = (1 << 4),
            F_Export      = (1 << 5),
            F_EntryPoint  = (1 << 6),
            F_Pointer     = (1 << 7),
        };

    public:
        REDASM_OBJECT(BlockItem)

    public:
        BlockItem();
        bool contains(address_t address) const;
        bool typeIs(type_t type) const;
        bool hasFlag(flag_t flags) const;
        bool empty() const;
        size_t size() const;
        String displayRange() const;
        String displayType() const;

    public:
        address_t start, end; // [start, end]
        type_t type{BlockItem::T_Unexplored};
        flag_t flags{BlockItem::F_None};
};

} // namespace REDasm
