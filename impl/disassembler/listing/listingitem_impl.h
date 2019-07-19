#pragma once

#include <redasm/libs/visit_struct/visit_struct.hpp>
#include <unordered_set>
#include <redasm/disassembler/listing/listingitem.h>

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
        ListingItemImpl(address_t address, ListingItemType type, size_t index);
        void save(cereal::BinaryOutputArchive &a) const;
        void load(cereal::BinaryInputArchive &a);

    private:
        std::unique_ptr<ListingItemData> m_data;
        address_t m_address;
        ListingItemType m_type;
        size_t m_index;
};

} // namespace REDasm

VISITABLE_STRUCT(REDasm::ListingMetaItem, name, type);
VISITABLE_STRUCT(REDasm::ListingItemData, comments, autocomments, meta, type);
