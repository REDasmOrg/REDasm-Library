#pragma once

#include <redasm/libs/visit_struct/visit_struct.hpp>
#include <redasm/disassembler/listing/listingitem.h>
#include "../types/containers/templates/sortedlist_template.h"
#include <unordered_set>

namespace REDasm {

typedef std::unordered_set<String> ListingCommentSet;

struct ListingItemData
{
    ListingCommentSet comments, autocomments;
    ListingMetaItem meta;
    String type;
};

class ListingItemImpl
{
    PIMPL_DECLARE_Q(ListingItem)
    PIMPL_DECLARE_PUBLIC(ListingItem)

    public:
        ListingItemImpl();
        ListingItemImpl(address_t address, ListingItemType type);
        ListingItemImpl(address_t address, ListingItemType type, size_t index);
        void save(cereal::BinaryOutputArchive &a) const;
        void load(cereal::BinaryInputArchive &a);

    private:
        ListingItemData m_data;
        address_t m_address;
        ListingItemType m_type;
        size_t m_index;
};

typedef std::unique_ptr<ListingItem> ListingItemPtr;

// NEW
typedef ListingItemComparatorNewT<ListingItem> ListingItemComparatorNew;
typedef ListingItemFinderNewT<ListingItem> ListingItemConstFinderNew;

// OLD
typedef ListingItemComparatorT<ListingItemPtr> ListingItemPtrComparator;
typedef ListingItemComparatorT<const ListingItem*> ListingItemConstComparator;
typedef ListingItemComparatorT<ListingItem*> ListingItemComparator;
typedef ListingItemFinderT<ListingItemPtr> ListingItemPtrFinder;
typedef ListingItemFinderT<const ListingItem*> ListingItemConstFinder;
typedef ListingItemFinderT<ListingItem*> ListingItemFinder;

class SortedItemList: public SortedListTemplate<ListingItem*, ListingItemConstComparator> { };

} // namespace REDasm

VISITABLE_STRUCT(REDasm::ListingMetaItem, name, type);
VISITABLE_STRUCT(REDasm::ListingItemData, comments, autocomments, meta, type);
