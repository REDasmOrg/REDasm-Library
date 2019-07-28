#include "edge.h"
#include <impl/graph/edge_impl.h>

namespace REDasm {

Edge::Edge(): source(0), target(0) { }
Edge::Edge(Node source, Node target): source(source), target(target) { }
bool Edge::valid() const { return (source != 0) && (target != 0); }
const Node& Edge::opposite(Node n) const { return (n == source) ? target : source; }
bool Edge::operator ==(const Edge& e) const { return (source == e.source) && (target == e.target); }
bool Edge::operator !=(const Edge& e) const { return (source != e.source) || (target != e.target); }

EdgeList::EdgeList(): m_pimpl_p(new EdgeListImpl()) { }
size_t EdgeList::size() const { PIMPL_P(const EdgeList); return p->size(); }
const Edge &EdgeList::at(size_t idx) const { PIMPL_P(const EdgeList); return p->at(idx); }
bool EdgeList::empty() const { PIMPL_P(const EdgeList); return p->empty(); }
void EdgeList::append(const Edge &e) { PIMPL_P(EdgeList); return p->append(e); }
void EdgeList::remove(const Edge &e) { PIMPL_P(EdgeList); return p->remove(e); }
void EdgeList::clear() { PIMPL_P(EdgeList); return p->clear(); }
const Edge &EdgeList::operator[](size_t idx) const { return this->at(idx); }

} // namespace REDasm

namespace std {

size_t std::hash<REDasm::Edge>::operator()(const REDasm::Edge &edge) const { return edge.source ^ edge.target; }

} // namespace std
