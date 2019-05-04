#include "listingfunctions.h"
#include "../../graph/functiongraph.h"

namespace REDasm {

ListingFunctions::ListingFunctions(): ListingItemConstContainer() { }

ListingFunctions::~ListingFunctions()
{
    for(const auto& item : m_graphs)
        delete item.second;
}

const ListingItem *ListingFunctions::functionFromIndex(size_t idx) const
{
    auto it = std::find_if(m_graphs.begin(), m_graphs.end(), [idx](const FunctionGraphItem& item) -> bool {
        return item.second->containsItem(idx);
    });

    if(it == m_graphs.end())
        return nullptr;

    return it->first;
}

void ListingFunctions::invalidateGraphs() { m_graphs.clear(); }
const Graphing::FunctionGraph *ListingFunctions::graph(const ListingItem *item) const { auto it = m_graphs.find(item); return (it != m_graphs.end()) ? it->second : nullptr; }
Graphing::FunctionGraph *ListingFunctions::graph(const ListingItem *item) { return const_cast<Graphing::FunctionGraph*>(static_cast<const ListingFunctions*>(this)->graph(item)); }

void ListingFunctions::graph(const ListingItem *item, Graphing::FunctionGraph* fb)
{
    auto it = m_graphs.find(item);

    if(it != m_graphs.end())
        delete it->second;

    m_graphs[item] = fb;
}

void ListingFunctions::erase(const ListingItem *item)
{
    m_graphs.erase(item);
    ListingItemConstContainer::erase(item);
}

} // namespace REDasm
