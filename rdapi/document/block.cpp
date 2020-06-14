#include "block.h"
#include <rdcore/document/backend/blockcontainer.h>

bool RDBlockContainer_Find(const RDBlockContainer* c, rd_address address, RDBlock* block) { return CPTR(const BlockContainer, c)->find(address, block); }
bool RDBlockContainer_Get(const RDBlockContainer* c, size_t index, RDBlock* block) { return CPTR(const BlockContainer, c)->get(index, block); }
size_t RDBlockContainer_Index(const RDBlockContainer* c, const RDBlock* block) { return CPTR(const BlockContainer, c)->indexOf(block); }
size_t RDBlockContainer_Size(const RDBlockContainer* c) { return CPTR(const BlockContainer, c)->size(); }

bool RDBlock_Contains(const RDBlock* b, rd_address address) { return BlockContainer::contains(b, address); }
size_t RDBlock_Empty(const RDBlock* b) { return BlockContainer::empty(b); }
size_t RDBlock_Size(const RDBlock* b) { return BlockContainer::size(b); }

