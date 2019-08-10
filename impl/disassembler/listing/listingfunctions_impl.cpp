#include "listingfunctions_impl.h"
#include <redasm/graph/functiongraph.h>

namespace REDasm {

ListingFunctionsImpl::ListingFunctionsImpl(): SortedItemList() { }

ListingFunctionsImpl::~ListingFunctionsImpl()
{
    for(const auto& item : m_graphs)
        delete item.second;
}

void ListingFunctionsImpl::remove(ListingItem *item)
{
    m_graphs.erase(item);
    SortedItemList::erase(item);
}

ListingFunctionsImpl::FunctionGraphs::const_iterator ListingFunctionsImpl::findGraph(size_t idx) const
{
    return std::find_if(m_graphs.begin(), m_graphs.end(), [idx](const ListingFunctionsImpl::FunctionGraphItem& item) -> bool {
        return item.second->containsItem(idx);
    });
}

} // namespace REDasm
