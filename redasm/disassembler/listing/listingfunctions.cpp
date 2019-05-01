#include "listingfunctions.h"

namespace REDasm {

ListingFunctions::ListingFunctions(): ListingItemConstContainer() { }

const ListingItem *ListingFunctions::functionFromIndex(size_t idx) const
{
    auto it = std::find_if(m_bounds.begin(), m_bounds.end(), [idx](const std::pair<const ListingItem*, BoundsList>& item) -> bool {
        for(const auto& bound : item.second) {
            if(bound.contains(idx))
                return true;
        }

        return false;
    });

    if(it == m_bounds.end())
        return nullptr;

    return it->first;
}

void ListingFunctions::invalidateBounds() { m_bounds.clear(); }
bool ListingFunctions::containsBounds(const ListingItem *item) const { return m_bounds.find(item) != m_bounds.end(); }
const ListingFunctions::BoundsList &ListingFunctions::bounds(const ListingItem *item) const { return m_bounds.at(item); }

void ListingFunctions::bounds(const ListingItem *item, const ListingFunctions::BoundsItem& b)
{
    auto it = m_bounds.find(item);

    if(it == m_bounds.end())
        m_bounds[item] = { b };
    else
        m_bounds[item].push_front(b);
}

void ListingFunctions::erase(const ListingItem *item)
{
    m_bounds.erase(item);
    ListingItemConstContainer::erase(item);
}

} // namespace REDasm
