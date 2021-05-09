#include "functionbasicblock.h"
#include "../../../document/document.h"
#include <rdapi/config.h>

FunctionBasicBlock::FunctionBasicBlock(SafeDocument& document, RDGraphNode n, rd_address address): node(n), startaddress(address), endaddress(address), m_document(document) { }
bool FunctionBasicBlock::contains(rd_address address) const { return ((address >= startaddress) && (address <= endaddress)); }

rd_type FunctionBasicBlock::getTheme(RDGraphNode n) const
{
    auto it = m_themes.find(n);
    return (it != m_themes.end()) ? it->second : Theme_GraphEdge;
}

void FunctionBasicBlock::bFalse(RDGraphNode n) { m_themes[n] = Theme_Fail; }
void FunctionBasicBlock::bTrue(RDGraphNode n) { m_themes[n] = Theme_Success; }
