#include "functiongraph.h"
#include <impl/graph/functiongraph_impl.h>
#include "../support/utils.h"
#include "../context.h"

namespace REDasm {

FunctionBasicBlock::FunctionBasicBlock(): m_pimpl_p(new FunctionBasicBlockImpl()) { }
FunctionBasicBlock::FunctionBasicBlock(size_t startidx): m_pimpl_p(new FunctionBasicBlockImpl(startidx)) { }
Node FunctionBasicBlock::node() const { PIMPL_P(const FunctionBasicBlock); return p->m_node; }
size_t FunctionBasicBlock::startIndex() const { PIMPL_P(const FunctionBasicBlock); return p->m_startidx; }
size_t FunctionBasicBlock::endIndex() const { PIMPL_P(const FunctionBasicBlock); return p->m_endidx;  }
bool FunctionBasicBlock::contains(size_t index) const { return (index >= this->startIndex()) && (index <= this->endIndex()); }
bool FunctionBasicBlock::isEmpty() const {  return this->startIndex() > this->endIndex(); }
size_t FunctionBasicBlock::count() const { return this->isEmpty() ? 0 : ((this->endIndex() - this->startIndex()) + 1); }
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

void FunctionBasicBlock::setStartIndex(size_t idx) { PIMPL_P(FunctionBasicBlock); p->m_startidx = idx; }
void FunctionBasicBlock::setEndIndex(size_t idx) { PIMPL_P(FunctionBasicBlock); p->m_endidx = idx; }
void FunctionBasicBlock::setNode(size_t idx) { PIMPL_P(FunctionBasicBlock); p->m_node = idx; }

FunctionGraph::FunctionGraph(): Graph(new FunctionGraphImpl()) { }
bool FunctionGraph::containsItem(size_t index) const { PIMPL_P(const FunctionGraph); return p->containsItem(index); }
bool FunctionGraph::build(address_t address) { PIMPL_P(FunctionGraph); return p->build(address); }
bool FunctionGraph::build(const ListingItem *item) { PIMPL_P(FunctionGraph); return p->build(item); }

} // namespace REDasm
