#include "callgraphitem.h"

CallGraphItem::CallGraphItem(RDGraphNode n, rd_address address): m_node(n), m_address(address) { }
RDGraphNode CallGraphItem::node() const { return m_node; }
rd_address CallGraphItem::address() const { return m_address; }
void CallGraphItem::addCall(const ILExpression* e) { m_calls.emplace_back(ILExpression::clone(e)); }
const ILExpression* CallGraphItem::call(size_t idx) const { return m_calls.at(idx).get(); }
size_t CallGraphItem::size() const { return m_calls.size(); }
