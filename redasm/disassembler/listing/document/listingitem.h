#pragma once

#include "../../../types/api/api_base.h"
#include "../../../types/containers/list.h"
#include "../../../types/object.h"
#include "../../../types/string.h"
#include "../../../macros.h"
#include "../../../pimpl.h"

namespace REDasm {

struct ListingMetaItem { String name, type; };

struct LIBREDASM_API ListingItem
{
    enum Type: type_t {
        None        = 0,
        FirstItem,
        SegmentItem = FirstItem, EmptyItem, FunctionItem, TypeItem, SymbolItem, MetaItem, InstructionItem, UnexploredItem, SeparatorItem,
        LastItem    = SeparatorItem,
        AllItems
    };

    address_t address{0};
    type_t type{ListingItem::None};
    size_t index{0};

    ListingItem();
    ListingItem(address_t address, type_t type);
    ListingItem(address_t address, type_t type, size_t index);
    bool is(type_t t) const;
    bool isValid() const;
    String displayType();

    bool operator ==(const ListingItem& rhs) const;
    bool operator !=(const ListingItem& rhs) const;
    bool operator <(const ListingItem& rhs) const;
    bool operator <=(const ListingItem& rhs) const;
    bool operator >(const ListingItem& rhs) const;
    bool operator >=(const ListingItem& rhs) const;

    static String displayType(type_t type);
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
