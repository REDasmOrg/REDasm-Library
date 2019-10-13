#pragma once

#include "../../types/containers/list.h"
#include "../../types/object.h"
#include "../../types/string.h"
#include "../../macros.h"
#include "../../pimpl.h"

namespace REDasm {

struct ListingMetaItem { String name, type; };

enum class ListingItemType: size_t
{
    Undefined = 0,
    FirstItem,
    SegmentItem = FirstItem, EmptyItem, FunctionItem, TypeItem, SymbolItem, MetaItem, InstructionItem, SeparatorItem,
    LastItem = SeparatorItem,
    AllItems = REDasm::npos
};

struct ListingItemData;

class LIBREDASM_API ListingItem: public Object
{
    REDASM_OBJECT(ListingItem)

    public:
        ListingItem();
        ListingItem(address_t address, ListingItemType type);
        ListingItem(address_t address, ListingItemType type, size_t index);
        bool is(ListingItemType t) const;
        bool isValid() const;
        ListingItemData* data();
        void save(cereal::BinaryOutputArchive &a) const override;
        void load(cereal::BinaryInputArchive &a) override;

    public:
        address_t address_new;
        ListingItemType type_new;
        size_t index_new;
};

template<typename T> struct ListingItemComparatorT {
    bool operator()(const T& t1, const T& t2) const {
        if(t1->address_new == t2->address_new) {
            if(t1->type_new == t2->type_new)
                return t1->index_new < t2->index_new;
            return t1->type_new < t2->type_new;
        }
        return t1->address_new < t2->address_new;
    }
};

template<typename T> struct ListingItemFinderT {
    bool operator()(const T& t1, const T& t2) const {
        if(t1->address_new == t2->address_new)
            return t1->type_new < t2->type_new;
        return t1->address_new < t2->address_new;
    }
};

template<typename T> struct ListingItemComparatorNewT {
    bool operator()(const T& t1, const T& t2) const {
        if(t1.address_new == t2.address_new) {
            if(t1.type_new == t2.type_new)
                return t1.index_new < t2.index_new;
            return t1.type_new < t2.type_new;
        }
        return t1.address_new < t2.address_new;
    }
};

template<typename T> struct ListingItemFinderNewT {
    bool operator()(const T& t1, const T& t2) const {
        if(t1.address_new == t2.address_new)
            return t1.type_new < t2.type_new;
        return t1.address_new < t2.address_new;
    }
};

} // namespace REDasm
