#pragma once

#include "listingitem.h"
#include "../../pimpl.h"

namespace REDasm {

class FunctionGraph;
class FunctionBasicBlock;
class ListingFunctionsImpl;

class LIBREDASM_API ListingFunctions
{
    PIMPL_DECLARE_P(ListingFunctions)
    PIMPL_DECLARE_PRIVATE(ListingFunctions)

    public:
        ListingFunctions();
        ListingItem* at(size_t idx) const;
        void insert(ListingItem* item);
        size_t size() const;

    public:
        ListingItem *functionFromItem(ListingItem *item) const;
        const FunctionBasicBlock* basicBlockFromItem(ListingItem *item) const;
        const FunctionGraph* graph(ListingItem* item) const;
        FunctionGraph* graph(ListingItem* item);
        void graph(ListingItem *item, FunctionGraph* graph);
        void remove(ListingItem *item);
        void invalidateGraphs();
};

} // namespace REDasm
