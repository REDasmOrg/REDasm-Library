#include "listingfunctions_impl.h"
#include <redasm/graph/functiongraph.h>

namespace REDasm {

ListingFunctionsImpl::ListingFunctionsImpl(): SortedItemList() { }
ListingFunctionsImpl::~ListingFunctionsImpl() { this->invalidateGraphs(); }

void ListingFunctionsImpl::remove(ListingItem *item)
{
    m_graphs.erase(item);
    SortedItemList::erase(item);
}

ListingFunctionsImpl::FunctionGraphs::const_iterator ListingFunctionsImpl::findGraph(ListingItem* item) const
{
    return std::find_if(m_graphs.begin(), m_graphs.end(), [item](const ListingFunctionsImpl::FunctionGraphItem& fgi) -> bool {
        return fgi.second->containsItem(item);
    });
}

void ListingFunctionsImpl::invalidateGraphs()
{
    for(const auto& item : m_graphs)
        delete item.second;

    m_graphs.clear();
}

} // namespace REDasm
