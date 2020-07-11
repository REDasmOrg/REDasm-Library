#pragma once

#include "../../containers/sortedcontainer.h"
#include <rdapi/document/document.h>
#include <tuple>

struct DocumentItemSorter
{
    bool operator()(const RDDocumentItem& item1, const RDDocumentItem& item2) const {
        if(item1.address == item2.address) {
            if(item1.type == item2.type) return item1.index < item2.index;
            return item1.type < item2.type;
        }

        return item1.address < item2.address;
    }
};

struct DocumentItemComparator
{
    bool operator()(const RDDocumentItem& item1, const RDDocumentItem& item2) const {
        return std::tie(item1.address, item1.type, item1.index) ==
               std::tie(item2.address, item2.type, item2.index);
    }
};

class ItemContainer: public SortedContainer<RDDocumentItem, DocumentItemSorter, DocumentItemComparator>
{
    public:
        ItemContainer() = default;
        size_t instructionIndex(rd_address address) const;
        size_t functionIndex(rd_address address) const;
        size_t symbolIndex(rd_address address) const;
};

