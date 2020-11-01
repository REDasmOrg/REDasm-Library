#include "itemcontainer.h"
#include <tuple>

bool ItemContainer::equals(const RDDocumentItem* item1, const RDDocumentItem* item2)
{
    if(!item1 || !item2) return false;

    return std::tie(item1->address, item1->type, item1->index) ==
           std::tie(item2->address, item2->type, item2->index);
}
