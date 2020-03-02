#pragma once

#include "../../../support/event/eventargs.h"
#include "listingitem.h"

namespace REDasm {

class ListingDocumentChangedEventArgs: public EventArgs
{
    public:
        enum Action { Changed = 0, Inserted, Removed };

    public:
        ListingDocumentChangedEventArgs(const ListingItem& item, size_t index, size_t action = Action::Changed);
        bool isInserted() const;
        bool isRemoved() const;
        bool isChanged() const;

    public:
        ListingItem item;
        size_t action;
        size_t index;
};

} // namespace REDasm

