#include "listingfunctions.h"
#include "../../graph/functiongraph.h"
#include <impl/disassembler/listing/listingfunctions_impl.h>

namespace REDasm {

ListingFunctions::ListingFunctions(): m_pimpl_p(new ListingFunctionsImpl()) { }
ListingItem *ListingFunctions::at(size_t idx) const { PIMPL_P(const ListingFunctions); return p->at(idx); }
void ListingFunctions::insert(ListingItem *item) { PIMPL_P(ListingFunctions); p->insert(item); }
size_t ListingFunctions::size() const { PIMPL_P(const ListingFunctions); return p->size(); }

ListingItem *ListingFunctions::functionFromIndex(size_t idx) const
{
    PIMPL_P(const ListingFunctions);

    auto it = std::find_if(p->m_graphs.begin(), p->m_graphs.end(), [idx](const ListingFunctionsImpl::FunctionGraphItem& item) -> bool {
        return item.second->containsItem(idx);
    });

    if(it == p->m_graphs.end())
        return nullptr;

    return it->first;
}

void ListingFunctions::invalidateGraphs() { PIMPL_P(ListingFunctions); p->m_graphs.clear(); }

const FunctionGraph *ListingFunctions::graph(ListingItem *item) const
{
    PIMPL_P(const ListingFunctions);
    auto it = p->m_graphs.find(item);
    return (it != p->m_graphs.end()) ? it->second : nullptr;
}

FunctionGraph *ListingFunctions::graph(ListingItem *item) { return const_cast<FunctionGraph*>(static_cast<const ListingFunctions*>(this)->graph(item)); }

void ListingFunctions::graph(ListingItem *item, FunctionGraph* fb)
{
    PIMPL_P(ListingFunctions);
    auto it = p->m_graphs.find(item);

    if(it != p->m_graphs.end())
        delete it->second;

    p->m_graphs[item] = fb;
}

void ListingFunctions::remove(ListingItem *item) { PIMPL_P(ListingFunctions); p->remove(item); }

} // namespace REDasm
