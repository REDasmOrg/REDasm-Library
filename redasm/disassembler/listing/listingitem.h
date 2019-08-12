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
    SegmentItem, EmptyItem, FunctionItem, TypeItem, SymbolItem, MetaItem, InstructionItem, SeparatorItem,
    AllItems = REDasm::npos
};

class ListingItemImpl;
struct ListingItemData;

class LIBREDASM_API ListingItem: public Object
{
    REDASM_OBJECT(ListingItem)
    PIMPL_DECLARE_P(ListingItem)
    PIMPL_DECLARE_PRIVATE(ListingItem)

    public:
        ListingItem();
        ListingItem(address_t address, ListingItemType type, size_t index);
        bool is(ListingItemType t) const;
        address_t address() const;
        ListingItemType type() const;
        size_t index() const;
        ListingItemData* data() const;
        void save(cereal::BinaryOutputArchive &a) const override;
        void load(cereal::BinaryInputArchive &a) override;
};

template<typename T> struct ListingItemComparatorT {
    bool operator()(const T& t1, const T& t2) const {
        if(t1->address() == t2->address()) {
            if(t1->type() == t2->type())
                return t1->index() < t2->index();
            return t1->type() < t2->type();
        }
        return t1->address() < t2->address();
    }
};

template<typename T> struct ListingItemFinderT {
    bool operator()(const T& t1, const T& t2) const {
        if(t1->address() == t2->address())
            return t1->type() < t2->type();
        return t1->address() < t2->address();
    }
};

} // namespace REDasm
