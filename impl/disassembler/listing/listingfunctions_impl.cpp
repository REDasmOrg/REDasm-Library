#include "listingfunctions_impl.h"
#include <redasm/graph/functiongraph.h>

namespace REDasm {

ListingFunctionsImpl::ListingFunctionsImpl(): SortedItemList() { }

REDasm::ListingFunctionsImpl::~ListingFunctionsImpl()
{
    for(const auto& item : m_graphs)
        delete item.second;
}

void ListingFunctionsImpl::remove(ListingItem *item)
{
    m_graphs.erase(item);
    SortedItemList::erase(item);
}

} // namespace REDasm
