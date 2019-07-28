#include "node.h"
#include <impl/graph/node_impl.h>

namespace REDasm {

NodeList::NodeList(): m_pimpl_p(new NodeListImpl()) { }
size_t NodeList::size() const { PIMPL_P(const NodeList); return p->size(); }
Node NodeList::at(size_t idx) const { PIMPL_P(const NodeList); return p->at(idx); }
bool NodeList::empty() const { PIMPL_P(const NodeList); return p->empty(); }
void NodeList::append(Node n) { PIMPL_P(NodeList); p->append(n); }
void NodeList::erase(Node n) { PIMPL_P(NodeList); p->erase(n); }
void NodeList::clear() { PIMPL_P(NodeList); p->clear(); }

} // namespace REDasm
