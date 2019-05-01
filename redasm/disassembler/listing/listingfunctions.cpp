#include "listingfunctions.h"

namespace REDasm {

ListingFunctions::ListingFunctions(): ListingItemContainer() { }

ListingItem *ListingFunctions::functionFromIndex(size_t idx) const
{
    auto it = std::find_if(m_bounds.begin(), m_bounds.end(), [idx](const std::pair<ListingItem*, BoundsList>& item) -> bool {
        for(const auto& bound : item.second) {
            if((idx < bound.first) || (idx > bound.second))
                continue;
            return true;
        }

        return false;
    });

    if(it == m_bounds.end())
        return nullptr;

    return it->first;
}

void ListingFunctions::invalidateBounds() { m_bounds.clear(); }

void ListingFunctions::bounds(ListingItem *item, const std::pair<size_t, size_t> &b)
{
    auto it = m_bounds.find(item);

    if(it == m_bounds.end())
        m_bounds[item] = { b };
    else
        m_bounds[item].push_front(b);
}

void ListingFunctions::erase(ListingItem *item)
{
    m_bounds.erase(item);
    ListingItemContainer::erase(item);
}

} // namespace REDasm
