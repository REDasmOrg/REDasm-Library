#pragma once

#include <redasm/disassembler/listing/listingfunctions.h>
#include <redasm/disassembler/listing/listingitem.h>
#include <redasm/pimpl.h>
#include <unordered_map>
#include "listingitem_impl.h"

namespace REDasm {

class ListingFunctionsImpl: public SortedItemList
{
    PIMPL_DECLARE_Q(ListingFunctions)
    PIMPL_DECLARE_PUBLIC(ListingFunctions)

    public:
        typedef std::pair<ListingItem*, FunctionGraph*> FunctionGraphItem;
        typedef std::unordered_map<ListingItem*, FunctionGraph*> FunctionGraphs;

    public:
        ListingFunctionsImpl();
        ~ListingFunctionsImpl();
        void remove(ListingItem *item);

    private:
        FunctionGraphs m_graphs;
};

} // namespace REDasm
