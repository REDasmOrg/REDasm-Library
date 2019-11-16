#pragma once

#include <redasm/libs/visit_struct/visit_struct.hpp>
#include <redasm/disassembler/listing/document/listingitem.h>
#include "../types/containers/templates/sortedlist_template.h"
#include <map>
#include <unordered_set>

namespace REDasm {

typedef std::unordered_set<String> ListingCommentSet;

struct ListingItemData
{
    ListingCommentSet comments, autocomments;
    std::map<size_t, ListingMetaItem> meta;
    String type;
};

class ListingItemImpl
{
    PIMPL_DECLARE_Q(ListingItem)
    PIMPL_DECLARE_PUBLIC(ListingItem)

    public:
        ListingItemImpl() = default;
        ListingItemImpl(address_t address, ListingItemType type);
        ListingItemImpl(address_t address, ListingItemType type, size_t index);
        void save(cereal::BinaryOutputArchive &a) const;
        void load(cereal::BinaryInputArchive &a);

    private:
        address_t m_address{0};
        ListingItemType m_type{ListingItemType::None};
        size_t m_index{0};
};

typedef ListingItemFinderNewT<ListingItem> ListingItemConstFinderNew;

} // namespace REDasm

VISITABLE_STRUCT(REDasm::ListingMetaItem, name, type);
VISITABLE_STRUCT(REDasm::ListingItemData, comments, autocomments, meta, type);
