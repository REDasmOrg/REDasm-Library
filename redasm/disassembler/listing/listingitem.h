#pragma once

#include <memory>
#include <unordered_set>
#include <deque>
#include "../../support/containers/sorted_container.h"
#include "../../redasm.h"

namespace REDasm {

namespace Detail {

struct MetaItem { std::string name, type; };
typedef std::unordered_set<std::string> CommentSet;

struct ListingItemData {
    CommentSet comments, autocomments;
    MetaItem meta;
    std::string type;
};

} // namespace Detail

struct ListingItem
{
    enum: size_t {
        Undefined = 0,
        SegmentItem, EmptyItem, FunctionItem, TypeItem, SymbolItem, MetaItem, InstructionItem,
        AllItems = static_cast<size_t>(-1)
    };

    ListingItem(): address(0), type(ListingItem::Undefined), index(0) { }
    ListingItem(address_t address, size_t type, size_t index): address(address), type(type), index(index) { data = std::make_unique<Detail::ListingItemData>(); }
    inline bool is(size_t t) const { return type == t; }

    std::unique_ptr<Detail::ListingItemData> data;
    address_t address;
    size_t type, index;
};

typedef std::unique_ptr<ListingItem> ListingItemPtr;
typedef std::deque<ListingItem*> ListingItems;

template<typename T> struct ListingItemComparatorT {
    bool operator()(const T& t1, const T& t2) const {
        if(t1->address == t2->address) {
            if(t1->type == t2->type)
                return t1->index < t2->index;
            return t1->type < t2->type;
        }
        return t1->address < t2->address;
    }
};

template<typename T> struct ListingItemFinderT {
    bool operator()(const T& t1, const T& t2) const {
        if(t1->address == t2->address)
            return t1->type < t2->type;
        return t1->address < t2->address;
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


} // namespace REDasm
