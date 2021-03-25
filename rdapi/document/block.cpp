#include "block.h"
#include <rdcore/document/model/blockcontainer.h>
#include <rdcore/object.h>

bool RDBlock_Contains(const RDBlock* b, rd_address address) { return BlockContainer::contains(b, address); }
size_t RDBlock_Empty(const RDBlock* b) { return BlockContainer::empty(b); }
size_t RDBlock_Size(const RDBlock* b) { return BlockContainer::size(b); }
