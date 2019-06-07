#pragma once

#include <redasm/libs/visit_struct/visit_struct.hpp>
#include <unordered_set>
#include <redasm/disassembler/listing/listingitem.h>

namespace REDasm {

typedef std::unordered_set<std::string> ListingCommentSet;

struct ListingItemData
{
    ListingCommentSet comments, autocomments;
    ListingMetaItem meta;
    std::string type;
};

class ListingItemImpl
{
    PIMPL_DECLARE_PUBLIC(ListingItem)

    public:
        ListingItemImpl();
        ListingItemImpl(address_t address, ListingItemType type, size_t index);

    private:
        std::unique_ptr<ListingItemData> m_data;
        address_t m_address;
        ListingItemType m_type;
        size_t m_index;

    friend class Serializer<ListingItem>;
};

} // namespace REDasm

VISITABLE_STRUCT(REDasm::ListingMetaItem, name, type);
VISITABLE_STRUCT(REDasm::ListingItemData, comments, autocomments, meta, type);

