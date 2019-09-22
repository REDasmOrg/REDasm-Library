#pragma once

#include <redasm/disassembler/listing/backend/listingitems.h>
#include "../../../types/containers/templates/sortedlist_template.h"
#include "../listingitem_impl.h"

namespace REDasm {

class ListingItemsImpl: public SortedListTemplate<ListingItem, ListingItemComparatorNew>
{
    public:
        ListingItemsImpl();
};

} // namespace REDasm
