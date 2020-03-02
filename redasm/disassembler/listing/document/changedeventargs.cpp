#include "changedeventargs.h"

namespace REDasm {

ListingDocumentChangedEventArgs::ListingDocumentChangedEventArgs(const ListingItem& item, size_t index, size_t action): item(item), action(action), index(index) { }
bool ListingDocumentChangedEventArgs::isInserted() const { return action == Action::Inserted; }
bool ListingDocumentChangedEventArgs::isRemoved() const { return action == Action::Removed; }
bool ListingDocumentChangedEventArgs::isChanged() const { return action == Action::Changed; }

} // namespace REDasm
