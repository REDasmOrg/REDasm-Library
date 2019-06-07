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
    PIMPL_DECLARE_PRIVATE(ListingFunctions)

    public:
        ListingFunctions();
        const ListingItem* at(size_t idx) const;
        void insert(const ListingItem* item);
        size_t size() const;

    public:
        const ListingItem *functionFromIndex(size_t idx) const;
        const Graphing::FunctionGraph* graph(const ListingItem* item) const;
        Graphing::FunctionGraph* graph(const ListingItem* item);
        void graph(const ListingItem *item, Graphing::FunctionGraph* graph);
        void erase(const ListingItem *item);
        void invalidateGraphs();
};

} // namespace REDasm
