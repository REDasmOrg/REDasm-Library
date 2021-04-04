#include "functionbasicblock.h"
#include "../../../document/document.h"
#include <rdapi/config.h>

FunctionBasicBlock::FunctionBasicBlock(SafeDocument& document, RDGraphNode n, rd_address address): node(n), startaddress(address), endaddress(address), m_document(document) { }
bool FunctionBasicBlock::contains(rd_address address) const { return ((address >= startaddress) && (address <= endaddress)); }

size_t FunctionBasicBlock::itemsCount() const
{
    if(!m_itemscount)
    {
        auto* blocks = m_document->getBlocks(startaddress);
        if(blocks != m_document->getBlocks(startaddress)) return 0; // NOTE: Can a basic block cross segments?

        auto startit = blocks->find(startaddress);
        auto endit = blocks->find(endaddress);
        if((startit == blocks->end()) || (endit == blocks->end())) return 0;

        m_itemscount = std::distance(startit, endit) + 1;
    }

    return m_itemscount;
}

rd_type FunctionBasicBlock::getTheme(RDGraphNode n) const
{
    auto it = m_themes.find(n);
    return (it != m_themes.end()) ? it->second : Theme_Default;
}

void FunctionBasicBlock::bFalse(RDGraphNode n) { m_themes[n] = Theme_Fail; }
void FunctionBasicBlock::bTrue(RDGraphNode n) { m_themes[n] = Theme_Success; }
