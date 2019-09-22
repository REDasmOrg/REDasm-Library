#pragma once

#include "../../../types/string.h"
#include "../../../types/base.h"
#include "blockitemflags.h"

namespace REDasm {

class BlockItem
{
    public:
        BlockItem();
        bool contains(address_t address) const;
        bool typeIs(BlockItemType type) const;
        bool hasFlag(BlockItemFlags flags) const;
        bool empty() const;
        size_t size() const;

    public:
        address_t start, end;
        BlockItemType type;
        BlockItemFlags flags;
        String name;
};

} // namespace REDasm
