#include "listingfunctions_impl.h"
#include <redasm/disassembler/model/functiongraph.h>

namespace REDasm {

ListingFunctionsImpl::ListingFunctionsImpl(): SortedSetTemplate<address_t>() { }
ListingFunctionsImpl::~ListingFunctionsImpl() { this->invalidateGraphs(); }

void ListingFunctionsImpl::remove(address_t address)
{
    m_graphs.erase(address);
    SortedSetTemplate<address_t>::remove(address);
}

ListingFunctionsImpl::FunctionGraphs::const_iterator ListingFunctionsImpl::findGraph(address_t address) const
{
    return std::find_if(m_graphs.begin(), m_graphs.end(), [address](const ListingFunctionsImpl::FunctionGraphItem& fgi) -> bool {
        return fgi.second->contains(address);
    });
}

void ListingFunctionsImpl::invalidateGraphs()
{
    for(const auto& item : m_graphs)
        delete item.second;

    m_graphs.clear();
}

} // namespace REDasm
