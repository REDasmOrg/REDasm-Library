#pragma once

#include "../../containers/treecontainer.h"
#include <rdapi/document/document.h>

struct DocumentItemSorter
{
    typedef void is_transparent;

    bool operator()(const RDDocumentItem& item1, const RDDocumentItem& item2) const {
        if(item1.address == item2.address) {
            if(item1.type == item2.type) return item1.index < item2.index;
            return item1.type < item2.type;
        }

        return item1.address < item2.address;
    }

    bool operator()(rd_address address, const RDDocumentItem& item) const { return address < item.address; }
    bool operator()(const RDDocumentItem& item1, rd_address address) const { return item1.address < address; }
};

class ItemContainer: public MultiTreeContainer<RDDocumentItem, DocumentItemSorter>
{
    public:
        ItemContainer() = default;
        bool containsInstruction(rd_address address) const { return this->containsItem(address, DocumentItemType_Instruction); }
        bool containerFunction(rd_address address) const { return this->containsItem(address, DocumentItemType_Instruction); }
        bool containsSymbol(rd_address address) const { return this->containsItem(address, DocumentItemType_Instruction); }

    private:
        bool containsItem(rd_address address, rd_type type, u16 index = 0) const { return this->m_container.count({ address, type, index }); }

    public: // vvv Deprececated vvv
        size_t instructionIndex(rd_address address) const;
        size_t functionIndex(rd_address address) const;
        size_t symbolIndex(rd_address address) const;
};
