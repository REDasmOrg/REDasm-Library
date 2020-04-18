#include "block.h"
#include <rdcore/document/backend/blockcontainer.h>

bool RDBlock_Contains(const RDBlock* b, address_t address) { return BlockContainer::contains(b, address); }
size_t RDBlock_Empty(const RDBlock* b) { return BlockContainer::empty(b); }
size_t RDBlock_Size(const RDBlock* b) { return BlockContainer::size(b); }
