#pragma once

#include "../listing/document/listingitem.h"
#include "../../types/containers/tree.h"
#include "../../pimpl.h"

namespace REDasm {

class CallTreeImpl;

class CallTree: public Tree<ListingItem>
{
    PIMPL_DECLARE_P(CallTree)
    PIMPL_DECLARE_PRIVATE(CallTree)

    public:
        CallTree();
        CallTree(const ListingItem& item);
        bool hasCalls() const;
        size_t populate();
};

typedef CallTree CallNode;

} // namespace REDasm
