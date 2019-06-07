#pragma once

#include <redasm/disassembler/listing/listingfunctions.h>
#include <redasm/disassembler/listing/listingitem.h>
#include <redasm/pimpl.h>
#include <unordered_map>

namespace REDasm {

class ListingFunctionsImpl: public ListingItemConstContainer
{
    PIMPL_DECLARE_PUBLIC(ListingFunctions)

    public:
        typedef std::pair< const ListingItem*, Graphing::FunctionGraph* > FunctionGraphItem;
        typedef std::unordered_map< const ListingItem*, Graphing::FunctionGraph* > FunctionGraphs;

    public:
        ListingFunctionsImpl();
        ~ListingFunctionsImpl();
        void erase(const ListingItem *item);

    private:
        FunctionGraphs m_graphs;
};

} // namespace REDasm
