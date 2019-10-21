#pragma once

#include "../listingitem.h"
#include "../../../macros.h"
#include "../../../pimpl.h"

namespace REDasm {

class ListingItemsImpl;

class ListingItems
{
    PIMPL_DECLARE_P(ListingItems)
    PIMPL_DECLARE_PRIVATE(ListingItems)

    public:
        ListingItems();
        bool empty() const;
        size_t size() const;
        size_t insert(address_t address, ListingItemType type, size_t index);
        size_t insert(const ListingItem& item);
        size_t indexOf(address_t address, ListingItemType type, size_t index = 0) const;
        size_t indexOf(const ListingItem& item) const;
        const ListingItem& at(size_t idx) const;
        ListingItem& at(size_t idx);
        void erase(const ListingItem& item);
        void erase(size_t idx);

    public:
        size_t itemIndex(address_t address) const;
        size_t listingIndex(address_t address) const;
        size_t segmentIndex(address_t address, size_t index = 0) const;
        size_t functionIndex(address_t address, size_t index = 0) const;
        size_t instructionIndex(address_t address, size_t index = 0) const;
        size_t symbolIndex(address_t address, size_t index = 0) const;
        ListingItem listingItem(address_t address) const;
        ListingItem segmentItem(address_t address) const;
        ListingItem functionItem(address_t address) const;
        ListingItem instructionItem(address_t address) const;
        ListingItem symbolItem(address_t address) const;
};

} // namespace REDasm
