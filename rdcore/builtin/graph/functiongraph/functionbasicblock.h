#pragma once

#include "../../../document/document_fwd.h"
#include "../../../graph/styledgraph.h"

struct FunctionBasicBlock
{
    FunctionBasicBlock(SafeDocument& document, RDGraphNode n, rd_address startaddress);
    bool contains(rd_address address) const;
    rd_type getTheme(RDGraphNode n) const;
    void bFalse(RDGraphNode n);
    void bTrue(RDGraphNode n);

    RDGraphNode node{0};
    rd_address startaddress, endaddress; // [startaddress, endaddress]

    private:
        mutable size_t m_itemscount{0};
        std::unordered_map<RDGraphNode, rd_type> m_themes;
        SafeDocument& m_document;
};
