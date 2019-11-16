#pragma once

#include <redasm/disassembler/listing/backend/listingitems.h>
#include "../../../types/containers/templates/sortedlist_template.h"
#include "../document/listingitem_impl.h"

namespace REDasm {

class ListingItemsImpl: public SortedListTemplate<ListingItem>
{
    public:
        ListingItemsImpl();
};

} // namespace REDasm
