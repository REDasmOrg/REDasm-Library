#include "listingitem_impl.h"
#include "../../../libs/cereal/cereal.hpp"
#include "../../../libs/cereal/types/unordered_set.hpp"
#include "../../../libs/cereal/archives/binary.hpp"
#include <redasm/macros.h>

namespace REDasm {

ListingItemImpl::ListingItemImpl(address_t address, ListingItemType type): m_address(address), m_type(type), m_index(0) { }
ListingItemImpl::ListingItemImpl(address_t address, ListingItemType type, size_t index): m_address(address), m_type(type), m_index(index) { }

void ListingItemImpl::save(cereal::BinaryOutputArchive &a) const
{
    //a(m_address, m_type, m_index,
      //m_data.comments, m_data.autocomments,
      //m_data.meta.name, m_data.meta.type,
      //m_data.type);
}

void ListingItemImpl::load(cereal::BinaryInputArchive &a)
{
    //a(m_address, m_type, m_index,
      //m_data.comments, m_data.autocomments,
      //m_data.meta.name, m_data.meta.type,
      //m_data.type);
}

} // namespace REDasm
