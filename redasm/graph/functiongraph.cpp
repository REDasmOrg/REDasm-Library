#include "functiongraph.h"
#include <impl/graph/functiongraph_impl.h>
#include "../support/utils.h"
#include "../context.h"

namespace REDasm {

FunctionBasicBlock::FunctionBasicBlock(): m_pimpl_p(new FunctionBasicBlockImpl()) { }
FunctionBasicBlock::FunctionBasicBlock(Node n, ListingItem* startitem): m_pimpl_p(new FunctionBasicBlockImpl(n, startitem)) { }
Node FunctionBasicBlock::node() const { PIMPL_P(const FunctionBasicBlock); return p->m_node; }
ListingItem* FunctionBasicBlock::startItem() const { PIMPL_P(const FunctionBasicBlock); return p->m_startitem; }
ListingItem* FunctionBasicBlock::endItem() const { PIMPL_P(const FunctionBasicBlock); return p->m_enditem;  }
ListingItem* FunctionBasicBlock::instructionStartItem() const { PIMPL_P(const FunctionBasicBlock); return p->m_startinstructionitem; }
ListingItem* FunctionBasicBlock::instructionEndItem() const { PIMPL_P(const FunctionBasicBlock); return p->m_endinstructionitem; }
size_t FunctionBasicBlock::startIndex() const { PIMPL_P(const FunctionBasicBlock); return r_doc->itemIndex(p->m_startitem);  }
size_t FunctionBasicBlock::endIndex() const { PIMPL_P(const FunctionBasicBlock); return r_doc->itemIndex(p->m_enditem);  }
size_t FunctionBasicBlock::instructionStartIndex() const { PIMPL_P(const FunctionBasicBlock); return r_doc->itemIndex(p->m_startinstructionitem); }
size_t FunctionBasicBlock::instructionEndIndex() const { PIMPL_P(const FunctionBasicBlock); return r_doc->itemIndex(p->m_endinstructionitem); }
bool FunctionBasicBlock::contains(ListingItem* item) const { PIMPL_P(const FunctionBasicBlock); return (item->address() >= p->m_startitem->address()) && (item->address() <= p->m_enditem->address()); }
bool FunctionBasicBlock::isEmpty() const { PIMPL_P(const FunctionBasicBlock); return (!p->m_startitem || !p->m_enditem); }
size_t FunctionBasicBlock::count() const { return this->isEmpty() ? 0 : ((this->endIndex() - this->startIndex()) + 1); }
size_t FunctionBasicBlock::instructionsCount() const { return (this->instructionEndItem() - this->instructionStartItem()) + 1; }
void FunctionBasicBlock::bTrue(Node n) { PIMPL_P(FunctionBasicBlock); p->m_styles[n] = "graph_edge_true"; }
void FunctionBasicBlock::bFalse(Node n) { PIMPL_P(FunctionBasicBlock); p->m_styles[n] = "graph_edge_false"; }

String FunctionBasicBlock::style(Node n) const
{
    PIMPL_P(const FunctionBasicBlock);
    auto it = p->m_styles.find(n);

    if(it == p->m_styles.end())
        return "graph_edge";

    return it->second;
}

void FunctionBasicBlock::setStartItem(ListingItem *item) { PIMPL_P(FunctionBasicBlock); p->m_startitem = item; }
void FunctionBasicBlock::setEndItem(ListingItem *item) { PIMPL_P(FunctionBasicBlock); p->m_enditem = item; }

void FunctionBasicBlock::setInstructionStartItem(ListingItem *item)
{
    PIMPL_P(FunctionBasicBlock);
    p->m_startinstructionitem = item;

    if(!p->m_endinstructionitem)
        p->m_endinstructionitem = item;
}

void FunctionBasicBlock::setInstructionEndItem(ListingItem *item) { PIMPL_P(FunctionBasicBlock); p->m_endinstructionitem = item; }
void FunctionBasicBlock::setNode(Node idx) { PIMPL_P(FunctionBasicBlock); p->m_node = idx; }

FunctionGraph::FunctionGraph(): Graph(new FunctionGraphImpl()) { }
const FunctionBasicBlock *FunctionGraph::basicBlockFromIndex(ListingItem* item) const { PIMPL_P(const FunctionGraph); return p->basicBlockFromItem(item); }
size_t FunctionGraph::bytesCount() const { PIMPL_P(const FunctionGraph); return p->bytesCount(); }
bool FunctionGraph::complete() const { PIMPL_P(const FunctionGraph); return p->complete(); }
bool FunctionGraph::containsItem(ListingItem* item) const { PIMPL_P(const FunctionGraph); return p->containsItem(item); }
bool FunctionGraph::build(address_t address) { PIMPL_P(FunctionGraph); return p->build(address); }
bool FunctionGraph::build(ListingItem *item) { PIMPL_P(FunctionGraph); return p->build(item); }

} // namespace REDasm
