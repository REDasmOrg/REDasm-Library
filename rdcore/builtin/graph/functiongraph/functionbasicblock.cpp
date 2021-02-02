#include "functionbasicblock.h"
#include "../../../document/document.h"
#include <rdapi/config.h>

FunctionBasicBlock::FunctionBasicBlock(SafeDocument& document, RDGraphNode n, rd_address address): node(n), startaddress(address), endaddress(address), m_document(document) { }
bool FunctionBasicBlock::contains(rd_address address) const { return ((address >= startaddress) && (address <= endaddress)); }

bool FunctionBasicBlock::getStartItem(RDDocumentItem* item) const
{
    if(m_document->items()->get(RDDocumentItem{ startaddress, DocumentItemType_Symbol, 0}, item))
    {
        RDSymbol symbol;
        if(m_document->symbol(startaddress, &symbol) && !IS_TYPE(&symbol, SymbolType_Function)) return true;
    }

    return m_document->items()->get(RDDocumentItem{ startaddress, DocumentItemType_Instruction, 0 }, item);
}

bool FunctionBasicBlock::getEndItem(RDDocumentItem* item) const
{
    static const std::array<rd_type, 4> END_ITEMS = {
        DocumentItemType_Instruction,

        // Include more types as fallback solution
        DocumentItemType_Symbol,
        DocumentItemType_Unknown,
        0
    };

    return m_document->getAny(endaddress, END_ITEMS.data(), item);
}

size_t FunctionBasicBlock::itemsCount() const
{
    if(!m_itemscount)
    {
        RDDocumentItem startitem, enditem;
        if(!this->getStartItem(&startitem) || !this->getEndItem(&enditem)) return 0;

        auto sit = m_document->items()->find(startitem);
        auto eit = m_document->items()->find(enditem);

        m_itemscount = std::distance(sit, eit) + 1;
    }

    return m_itemscount;
}

rd_type FunctionBasicBlock::getTheme(RDGraphNode n) const
{
    auto it = m_themes.find(n);
    return (it != m_themes.end()) ? it->second : Theme_Default;
}

void FunctionBasicBlock::bFalse(RDGraphNode n) { m_themes[n] = Theme_GraphEdgeFalse; }
void FunctionBasicBlock::bTrue(RDGraphNode n) { m_themes[n] = Theme_GraphEdgeTrue; }
