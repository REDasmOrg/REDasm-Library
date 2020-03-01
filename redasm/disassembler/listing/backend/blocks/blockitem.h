#pragma once

#include "../../../../types/base.h"
#include "../../../../types/string.h"
#include "blockitemflags.h"

namespace REDasm {

class LIBREDASM_API BlockItem: public Object
{
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
        type_t type{BlockItemType::Unexplored};
        flag_t flags{BlockItemFlags::None};
};

} // namespace REDasm
