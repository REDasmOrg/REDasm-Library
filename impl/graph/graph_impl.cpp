#include "graph_impl.h"

namespace REDasm {

GraphImpl::GraphImpl(): m_nodeid(0), m_areawidth(0), m_areaheight(0), m_root(0) { }
bool GraphImpl::empty() const { return m_nodes.empty(); }

bool GraphImpl::containsEdge(Node source, Node target) const
{
    for(size_t i = 0; i < m_edges.size(); i++)
    {
        const Edge& e = m_edges[i];

        if((e.source == source) && (e.target == target))
            return true;
    }

    return false;
}

void GraphImpl::removeEdge(const Edge& e) { m_edges.remove(e); }
void GraphImpl::removeNode(Node n) { m_nodes.remove(n); this->removeEdges(n); }

EdgeList GraphImpl::outgoing(Node n) const
{
    EdgeList oe;

    for(size_t i = 0; i < m_edges.size(); i++)
    {
        const Edge& e = m_edges[i];

        if(e.source != n)
            continue;

        oe.append(e);
    }

    return oe;
}

EdgeList GraphImpl::incoming(Node n) const
{
    EdgeList ie;

    for(size_t i = 0; i < m_edges.size(); i++)
    {
        const Edge& e = m_edges[i];

        if(e.target != n)
            continue;

        ie.append(e);
    }

    return ie;
}

const NodeList &GraphImpl::nodes() const { return m_nodes; }
const EdgeList &GraphImpl::edges() const { return m_edges; }

Edge GraphImpl::edge(Node source, Node target) const
{
    for(size_t i = 0; i < m_edges.size(); i++)
    {
        const Edge& e = m_edges[i];

        if((e.source == source) && (e.target == target))
            return e;
    }

    return Edge();
}

void GraphImpl::newEdge(Node source, Node target)
{
    if(!this->containsEdge(source, target))
        m_edges.append(Edge(source, target));
}

void GraphImpl::setRoot(Node n) { m_root = n; }
Node GraphImpl::root() const { return m_root; }
Node GraphImpl::newNode() { Node n = ++m_nodeid; m_nodes.append(n); return n; }
int GraphImpl::areaWidth() const { return m_areawidth; }
int GraphImpl::areaHeight() const { return m_areaheight; }
void GraphImpl::areaWidth(int w) { m_areawidth = w; }
void GraphImpl::areaHeight(int h) { m_areaheight = h; }
int GraphImpl::x(Node n) const { return m_nodeattributes.at(n).x; }
int GraphImpl::y(Node n) const { return m_nodeattributes.at(n).y; }
int GraphImpl::width(Node n) const { return m_nodeattributes.at(n).width; }
int GraphImpl::height(Node n) const { return m_nodeattributes.at(n).height; }
void GraphImpl::x(Node n, int px) { m_nodeattributes[n].x = px; }
void GraphImpl::y(Node n, int py) { m_nodeattributes[n].y = py; }
void GraphImpl::width(Node n, int w) { m_nodeattributes[n].width = w; }
void GraphImpl::height(Node n, int h) { m_nodeattributes[n].height = h; }
const String &GraphImpl::color(const Edge& e) const { return m_edgeattributes.at(e).color; }
const String &GraphImpl::label(const Edge& e) const { return m_edgeattributes.at(e).label; }
const Polyline &GraphImpl::routes(const Edge& e) const { return m_edgeattributes.at(e).routes; }
const Polyline &GraphImpl::arrow(const Edge& e) const { return m_edgeattributes.at(e).arrow; }
void GraphImpl::color(const Edge &e, const String &c) { m_edgeattributes[e].color = c; }
void GraphImpl::label(const Edge &e, const String &s) { m_edgeattributes[e].label = s; }
void GraphImpl::routes(const Edge &e, const Polyline &p) { m_edgeattributes[e].routes = p; }
void GraphImpl::arrow(const Edge &e, const Polyline &p) { m_edgeattributes[e].arrow = p; }
const Dictionary &GraphImpl::data() const { return m_data; }
Variant GraphImpl::data(Node n) const { return m_data.contains(n) ? m_data[n] : Variant(); }
void GraphImpl::setData(Node n, const Variant &v) { m_data[n] = v; }

void GraphImpl::removeEdges(Node n)
{
    for(size_t i = 0; i < m_edges.size(); )
    {
        const Edge& e = m_edges[i];

        if(e.source == n)
            this->removeEdge(e);
        else
            i++;
    }
}

} // namespace REDasm
