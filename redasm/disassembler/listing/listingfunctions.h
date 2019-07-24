#pragma once

#include <unordered_map>
#include "listingitem.h"
#include "../../pimpl.h"

namespace REDasm {

namespace Graphing {
class FunctionGraph;
}

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
        ListingItem *functionFromIndex(size_t idx) const;
        const Graphing::FunctionGraph* graph(ListingItem* item) const;
        Graphing::FunctionGraph* graph(ListingItem* item);
        void graph(ListingItem *item, Graphing::FunctionGraph* graph);
        void remove(ListingItem *item);
        void invalidateGraphs();
};

} // namespace REDasm
