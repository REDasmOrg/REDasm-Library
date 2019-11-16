#pragma once

#include <redasm/disassembler/listing/backend/listingfunctions.h>
#include <redasm/disassembler/listing/document/listingitem.h>
#include <redasm/pimpl.h>
#include <unordered_map>
#include "../document/listingitem_impl.h"
#include "../../../types/containers/templates/sortedset_template.h"

namespace REDasm {

class ListingFunctionsImpl: public SortedSetTemplate<address_t>
{
    PIMPL_DECLARE_Q(ListingFunctions)
    PIMPL_DECLARE_PUBLIC(ListingFunctions)

    public:
        typedef std::pair<address_t, FunctionGraph*> FunctionGraphItem;
        typedef std::unordered_map<address_t, FunctionGraph*> FunctionGraphs;

    public:
        ListingFunctionsImpl();
        ~ListingFunctionsImpl();
        void remove(address_t address);
        FunctionGraphs::const_iterator findGraph(address_t address) const;
        void invalidateGraphs();

    private:
        FunctionGraphs m_graphs;
};

} // namespace REDasm
