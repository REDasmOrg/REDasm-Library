#pragma once

#include "../../../types/api/api_base.h"
#include "../../../types/containers/list.h"
#include "../../../types/object.h"
#include "../../../types/string.h"
#include "../../../macros.h"
#include "../../../pimpl.h"

namespace REDasm {

struct ListingMetaItem { String name, type; };

enum class ListingItemType: size_t
{
    None        = 0,
    FirstItem,
    SegmentItem = FirstItem, EmptyItem, FunctionItem, TypeItem, SymbolItem, MetaItem, InstructionItem, UnexploredItem, SeparatorItem,
    LastItem    = SeparatorItem,
    AllItems    = REDasm::npos
};

struct LIBREDASM_API ListingItem
{
    address_t address{0};
    ListingItemType type{ListingItemType::None};
    size_t index{0};

    ListingItem();
    ListingItem(address_t address, ListingItemType type);
    ListingItem(address_t address, ListingItemType type, size_t index);
    bool is(ListingItemType t) const;
    bool isValid() const;
    String displayType();

    bool operator ==(const ListingItem& rhs) const;
    bool operator !=(const ListingItem& rhs) const;
    bool operator <(const ListingItem& rhs) const;
    bool operator <=(const ListingItem& rhs) const;
    bool operator >(const ListingItem& rhs) const;
    bool operator >=(const ListingItem& rhs) const;

    static String displayType(ListingItemType type);
};

FORCE_STANDARD_LAYOUT(ListingItem)

template<typename T> struct ListingItemFinderNewT {
    bool operator()(const T& t1, const T& t2) const {
        if(t1.address == t2.address)
            return t1.type < t2.type;
        return t1.address < t2.address;
    }
};

} // namespace REDasm
