#include "graph.h"
#include <algorithm>
#include <impl/graph/graph_impl.h>

namespace REDasm {

NodeAttributes::NodeAttributes(): x(0), y(0), width(0), height(0) { }

Graph::Graph(GraphImpl *p): m_pimpl_p(p) { }
Graph::Graph(): m_pimpl_p(new GraphImpl()) { }
bool Graph::empty() const { PIMPL_P(const Graph); return p->empty(); }
bool Graph::containsEdge(Node source, Node target) const { PIMPL_P(const Graph); return p->containsEdge(source, target); }
void Graph::removeEdge(const Edge &e) { PIMPL_P(Graph); p->removeEdge(e); }
void Graph::removeNode(Node n) { PIMPL_P(Graph); p->removeNode(n); }
EdgeList Graph::outgoing(Node n) const { PIMPL_P(const Graph); return p->outgoing(n); }
EdgeList Graph::incoming(Node n) const { PIMPL_P(const Graph); return p->incoming(n); }
const NodeList &Graph::nodes() const { PIMPL_P(const Graph); return p->nodes(); }
const EdgeList &Graph::edges() const { PIMPL_P(const Graph); return p->edges(); }
Edge Graph::edge(Node source, Node target) const { PIMPL_P(const Graph); return p->edge(source, target); }
void Graph::newEdge(Node source, Node target) { PIMPL_P(Graph); p->newEdge(source, target); }
Node Graph::newNode() { PIMPL_P(Graph); return p->newNode(); }
int Graph::areaWidth() const { PIMPL_P(const Graph); return p->areaWidth(); }
int Graph::areaHeight() const { PIMPL_P(const Graph); return p->areaHeight(); }
void Graph::areaWidth(int w) { PIMPL_P(Graph); p->areaWidth(w); }
void Graph::areaHeight(int h) { PIMPL_P(Graph); p->areaHeight(h); }
int Graph::x(Node n) const { PIMPL_P(const Graph); return p->x(n); }
int Graph::y(Node n) const { PIMPL_P(const Graph); return p->y(n); }
void Graph::x(Node n, int px) { PIMPL_P(Graph); p->x(n, px); }
void Graph::y(Node n, int py) { PIMPL_P(Graph); p->y(n, py); }
int Graph::width(Node n) const { PIMPL_P(const Graph); return p->width(n); }
int Graph::height(Node n) const { PIMPL_P(const Graph); return p->height(n); }
void Graph::width(Node n, int w) { PIMPL_P(Graph); return p->width(n, w); }
void Graph::height(Node n, int h) { PIMPL_P(Graph); return p->height(n, h); }
const String &Graph::color(const Edge &e) const { PIMPL_P(const Graph); return p->color(e); }
const String &Graph::label(const Edge &e) const { PIMPL_P(const Graph); return p->label(e); }
const Polyline &Graph::routes(const Edge &e) const { PIMPL_P(const Graph); return p->routes(e); }
const Polyline &Graph::arrow(const Edge &e) const { PIMPL_P(const Graph); return p->arrow(e); }
void Graph::color(const Edge &e, const String &c) { PIMPL_P(Graph); p->color(e, c); }
void Graph::label(const Edge &e, const String &s) { PIMPL_P(Graph); p->label(e, s); }
void Graph::routes(const Edge &e, const Polyline &pl) { PIMPL_P(Graph); p->routes(e, pl); }
void Graph::arrow(const Edge &e, const Polyline &pl) { PIMPL_P(Graph); p->arrow(e, pl); }
const Dictionary &Graph::data() const { PIMPL_P(const Graph); return p->data(); }
Variant Graph::data(Node n) const { PIMPL_P(const Graph); return p->data(n); }
void Graph::setData(Node n, const Variant &v) { PIMPL_P(Graph); p->setData(n, v); }
void Graph::setRoot(Node n) { PIMPL_P(Graph); p->setRoot(n); }
Node Graph::root() const { PIMPL_P(const Graph); return p->root(); }

} // namespace REDasm
