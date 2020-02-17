#include "changedeventargs.h"

namespace REDasm {

ListingDocumentChangedEventArgs::ListingDocumentChangedEventArgs(const ListingItem& item, size_t index, ListingDocumentAction action): item(item), action(action), index(index) { }
bool ListingDocumentChangedEventArgs::isInserted() const { return action == ListingDocumentAction::Inserted; }
bool ListingDocumentChangedEventArgs::isRemoved() const { return action == ListingDocumentAction::Removed; }
bool ListingDocumentChangedEventArgs::isChanged() const { return action == ListingDocumentAction::Changed; }

} // namespace REDasm
