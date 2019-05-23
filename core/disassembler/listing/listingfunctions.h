#pragma once

#include <unordered_map>
#include "listingitem.h"

namespace REDasm {

namespace Graphing {
class FunctionGraph;
}

class ListingFunctions: public ListingItemConstContainer
{
    private:
        typedef std::pair< const ListingItem*, Graphing::FunctionGraph* > FunctionGraphItem;
        typedef std::unordered_map< const ListingItem*, Graphing::FunctionGraph* > FunctionGraphs;

    public:
        ListingFunctions();
        ~ListingFunctions();
        const ListingItem* functionFromIndex(size_t idx) const;
        const Graphing::FunctionGraph* graph(const ListingItem* item) const;
        Graphing::FunctionGraph* graph(const ListingItem* item);
        void graph(const ListingItem* item, Graphing::FunctionGraph* graph);
        void erase(const ListingItem* item);
        void invalidateGraphs();

    private:
        FunctionGraphs m_graphs;
};

} // namespace REDasm
