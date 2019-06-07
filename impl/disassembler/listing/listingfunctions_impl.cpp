#include "listingfunctions_impl.h"
#include <redasm/graph/functiongraph.h>

namespace REDasm {

ListingFunctionsImpl::ListingFunctionsImpl(): ListingItemConstContainer() { }

REDasm::ListingFunctionsImpl::~ListingFunctionsImpl()
{
    for(const auto& item : m_graphs)
        delete item.second;
}

void ListingFunctionsImpl::erase(const ListingItem *item)
{
    m_graphs.erase(item);
    ListingItemConstContainer::erase(item);
}

} // namespace REDasm
