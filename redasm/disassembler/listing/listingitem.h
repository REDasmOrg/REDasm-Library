#pragma once

#include <redasm/support/containers/sorted_container.h>
#include <deque>
#include "../../support/serializer.h"
#include "../../types/base_types.h"
#include "../../pimpl.h"

namespace REDasm {

struct ListingMetaItem { std::string name, type; };

enum class ListingItemType: size_t
{
    Undefined = 0,
    SegmentItem, EmptyItem, FunctionItem, TypeItem, SymbolItem, MetaItem, InstructionItem,
    AllItems = REDasm::npos
};

class ListingItemImpl;
struct ListingItemData;

class LIBREDASM_API ListingItem
{
    PIMPL_DECLARE_PRIVATE(ListingItem)

    public:
        ListingItem();
        ListingItem(address_t address, ListingItemType type, size_t index);
        bool is(ListingItemType t) const;
        address_t address() const;
        ListingItemType type() const;
        size_t index() const;
        ListingItemData* data() const;

    friend class Serializer<ListingItem>;
};

typedef std::unique_ptr<ListingItem> ListingItemPtr;
typedef std::deque<ListingItem*> ListingItems;

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

typedef ListingItemComparatorT<ListingItemPtr> ListingItemPtrComparator;
typedef ListingItemComparatorT<const ListingItem*> ListingItemConstComparator;
typedef ListingItemComparatorT<ListingItem*> ListingItemComparator;
typedef ListingItemFinderT<ListingItemPtr> ListingItemPtrFinder;
typedef ListingItemFinderT<const ListingItem*> ListingItemConstFinder;
typedef ListingItemFinderT<ListingItem*> ListingItemFinder;
typedef sorted_container<const ListingItem*, ListingItemConstComparator> ListingItemConstContainer;
typedef sorted_container<ListingItem*, ListingItemComparator> ListingItemContainer;

template<> struct Serializer<ListingItem> {
    static void write(std::fstream& fs, const ListingItem& d);
    static void read(std::fstream& fs, ListingItem& d);
};

} // namespace REDasm
