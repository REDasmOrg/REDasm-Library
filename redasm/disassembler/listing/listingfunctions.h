#pragma once

#include <unordered_map>
#include <forward_list>
#include "listingitem.h"

namespace REDasm {

class ListingFunctions: public ListingItemContainer
{
    public:
        typedef std::pair<size_t, size_t> BoundsItem;
        typedef std::forward_list<BoundsItem> BoundsList;
        typedef std::unordered_map<ListingItem*, BoundsList> FunctionBounds;

    public:
        ListingFunctions();
        ListingItem* functionFromIndex(size_t idx) const;
        void invalidateBounds();
        void bounds(ListingItem* item, const std::pair<size_t, size_t>& b);
        void erase(ListingItem* item);

    private:
        FunctionBounds m_bounds;
};

} // namespace REDasm
