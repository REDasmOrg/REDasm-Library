#include "listingitem_impl.h"
#include <redasm/macros.h>

namespace REDasm {

ListingItemImpl::ListingItemImpl(): m_address(0), m_type(ListingItemType::Undefined), m_index(0)
{
    m_data = std::make_unique<ListingItemData>();
}

ListingItemImpl::ListingItemImpl(address_t address, ListingItemType type, size_t index): m_address(address), m_type(type), m_index(index)
{
    m_data = std::make_unique<ListingItemData>();
}

} // namespace REDasm
