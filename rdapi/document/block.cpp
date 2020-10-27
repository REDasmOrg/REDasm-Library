#include "block.h"
#include <rdcore/document/backend/blockcontainer.h>
#include <rdcore/object.h>

void RDBlockContainer_Each(const RDBlockContainer* c, BlockContainer_Callback cb, void* userdata)
{
    CPTR(const BlockContainer, c)->each([&](const RDBlock& b) {
        return cb(&b, userdata);
    });
}

bool RDBlockContainer_Get(const RDBlockContainer* c, rd_address address, RDBlock* block) { return CPTR(const BlockContainer, c)->get(address, block); }
size_t RDBlockContainer_Size(const RDBlockContainer* c) { return CPTR(const BlockContainer, c)->size(); }

bool RDBlock_Contains(const RDBlock* b, rd_address address) { return BlockContainer::contains(b, address); }
size_t RDBlock_Empty(const RDBlock* b) { return BlockContainer::empty(b); }
size_t RDBlock_Size(const RDBlock* b) { return BlockContainer::size(b); }
