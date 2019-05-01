#pragma once

#include <unordered_map>
#include <forward_list>
#include "listingitem.h"

namespace REDasm {

class ListingFunctions: public ListingItemConstContainer
{
    public:
        struct BoundsItem {
            size_t startidx, endidx; // [startidx, endidx]
            bool contains(size_t idx) const { return (idx >= startidx) && (idx <= endidx); }
            size_t size() const { return (endidx >= startidx) ? (endidx - startidx) + 1 : 0; }
        };

        typedef std::forward_list<BoundsItem> BoundsList;
        typedef std::unordered_map<const ListingItem*, BoundsList> FunctionBounds;

    public:
        ListingFunctions();
        const ListingItem* functionFromIndex(size_t idx) const;
        void invalidateBounds();
        bool containsBounds(const ListingItem* item) const;
        const BoundsList& bounds(const ListingItem* item) const;
        void bounds(const ListingItem* item, const BoundsItem &b);
        void erase(const ListingItem* item);

    private:
        FunctionBounds m_bounds;
};

} // namespace REDasm
