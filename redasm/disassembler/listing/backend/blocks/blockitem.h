#pragma once

#include "../../../types/base.h"
#include "../../../types/string.h"
#include "blockitemflags.h"

namespace REDasm {

class LIBREDASM_API BlockItem: public Object
{
    public:
        REDASM_OBJECT(BlockItem)

    public:
        BlockItem();
        bool contains(address_t address) const;
        bool typeIs(BlockItemType type) const;
        bool hasFlag(BlockItemFlags flags) const;
        bool empty() const;
        size_t size() const;
        String displayRange() const;
        String displayType() const;

    public:
        address_t start, end; // [start, end]
        BlockItemType type;
        BlockItemFlags flags;
};

} // namespace REDasm
