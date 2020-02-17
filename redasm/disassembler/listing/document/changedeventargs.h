#pragma once

#include "../../../support/event/eventargs.h"
#include "listingitem.h"

namespace REDasm {

enum class ListingDocumentAction { Changed = 0, Inserted, Removed };

class ListingDocumentChangedEventArgs: public EventArgs
{
    public:
        ListingDocumentChangedEventArgs(const ListingItem& item, size_t index, ListingDocumentAction action = ListingDocumentAction::Changed);
        bool isInserted() const;
        bool isRemoved() const;
        bool isChanged() const;

    public:
        ListingItem item;
        ListingDocumentAction action;
        size_t index;
};

} // namespace REDasm

