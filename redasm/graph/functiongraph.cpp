#include "functiongraph.h"
#include <impl/graph/functiongraph_impl.h>
#include "../support/utils.h"
#include "../context.h"

namespace REDasm {

FunctionBasicBlock::FunctionBasicBlock(): m_pimpl_p(new FunctionBasicBlockImpl()) { }
FunctionBasicBlock::FunctionBasicBlock(Node n, const ListingItem& startitem): m_pimpl_p(new FunctionBasicBlockImpl(n, startitem)) { }
Node FunctionBasicBlock::node() const { PIMPL_P(const FunctionBasicBlock); return p->m_node; }
const ListingItem& FunctionBasicBlock::startItem() const { PIMPL_P(const FunctionBasicBlock); return p->m_startitem; }
const ListingItem& FunctionBasicBlock::endItem() const { PIMPL_P(const FunctionBasicBlock); return p->m_enditem;  }
const ListingItem& FunctionBasicBlock::instructionStartItem() const { PIMPL_P(const FunctionBasicBlock); return p->m_startinstructionitem; }
const ListingItem& FunctionBasicBlock::instructionEndItem() const { PIMPL_P(const FunctionBasicBlock); return p->m_endinstructionitem; }
size_t FunctionBasicBlock::startIndex() const { PIMPL_P(const FunctionBasicBlock); return r_docnew->itemIndex(p->m_startitem.address_new);  }
size_t FunctionBasicBlock::endIndex() const { PIMPL_P(const FunctionBasicBlock); return r_docnew->itemIndex(p->m_enditem.address_new);  }
size_t FunctionBasicBlock::instructionStartIndex() const { PIMPL_P(const FunctionBasicBlock); return r_docnew->itemInstructionIndex(p->m_startinstructionitem.address_new); }
size_t FunctionBasicBlock::instructionEndIndex() const { PIMPL_P(const FunctionBasicBlock); return r_docnew->itemInstructionIndex(p->m_endinstructionitem.address_new); }
bool FunctionBasicBlock::contains(address_t address) const { PIMPL_P(const FunctionBasicBlock); return (address >= p->m_startitem.address_new) && (address <= p->m_enditem.address_new); }
bool FunctionBasicBlock::isEmpty() const { PIMPL_P(const FunctionBasicBlock); return (!p->m_startitem.isValid() || !p->m_enditem.isValid()); }
size_t FunctionBasicBlock::count() const { return this->isEmpty() ? 0 : ((this->endIndex() - this->startIndex()) + 1); }
void FunctionBasicBlock::bTrue(Node n) { PIMPL_P(FunctionBasicBlock); p->m_styles[n] = "graph_edge_true"; }
void FunctionBasicBlock::bFalse(Node n) { PIMPL_P(FunctionBasicBlock); p->m_styles[n] = "graph_edge_false"; }

String FunctionBasicBlock::style(Node n) const
{
    PIMPL_P(const FunctionBasicBlock);
    auto it = p->m_styles.find(n);

    if(it == p->m_styles.end()) return "graph_edge";
    return it->second;
}

void FunctionBasicBlock::setStartItem(const ListingItem& item) { PIMPL_P(FunctionBasicBlock); p->m_startitem = item; }
void FunctionBasicBlock::setEndItem(const ListingItem& item) { PIMPL_P(FunctionBasicBlock); p->m_enditem = item; }

void FunctionBasicBlock::setInstructionStartItem(const ListingItem& item)
{
    PIMPL_P(FunctionBasicBlock);
    p->m_startinstructionitem = item;
    if(!p->m_endinstructionitem.isValid()) p->m_endinstructionitem = item;
}

void FunctionBasicBlock::setInstructionEndItem(const ListingItem& item) { PIMPL_P(FunctionBasicBlock); p->m_endinstructionitem = item; }
void FunctionBasicBlock::setNode(Node idx) { PIMPL_P(FunctionBasicBlock); p->m_node = idx; }

FunctionGraph::FunctionGraph(): Graph(new FunctionGraphImpl()) { }
const FunctionBasicBlock *FunctionGraph::basicBlockFromAddress(address_t address) const { PIMPL_P(const FunctionGraph); return p->basicBlockFromAddress(address); }
size_t FunctionGraph::bytesCount() const { PIMPL_P(const FunctionGraph); return p->bytesCount(); }
bool FunctionGraph::complete() const { PIMPL_P(const FunctionGraph); return p->complete(); }
bool FunctionGraph::contains(address_t address) const { PIMPL_P(const FunctionGraph); return p->contains(address); }
bool FunctionGraph::build(address_t address) { PIMPL_P(FunctionGraph); return p->build(address); }

} // namespace REDasm
