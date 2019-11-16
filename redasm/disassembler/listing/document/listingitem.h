#pragma once

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

class LIBREDASM_API ListingItem: public Object
{
    REDASM_OBJECT(ListingItem)

    public:
        ListingItem();
        ListingItem(address_t address, ListingItemType type);
        ListingItem(address_t address, ListingItemType type, size_t index);
        bool is(ListingItemType t) const;
        bool isValid() const;
        void save(cereal::BinaryOutputArchive &a) const override;
        void load(cereal::BinaryInputArchive &a) override;
        String displayType();

    public:
        bool operator ==(const ListingItem& rhs) const;
        bool operator !=(const ListingItem& rhs) const;
        bool operator <(const ListingItem& rhs) const;
        bool operator <=(const ListingItem& rhs) const;
        bool operator >(const ListingItem& rhs) const;
        bool operator >=(const ListingItem& rhs) const;

    public:
        static String displayType(ListingItemType type);

    public:
        address_t address;
        ListingItemType type;
        size_t index;
};

template<typename T> struct ListingItemFinderNewT {
    bool operator()(const T& t1, const T& t2) const {
        if(t1.address == t2.address)
            return t1.type < t2.type;
        return t1.address < t2.address;
    }
};

} // namespace REDasm
