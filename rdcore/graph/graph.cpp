#include "graph.h"
#include <algorithm>
#include <tuple>

bool Graph::empty() const { return m_nodes.empty(); }
void Graph::setRoot(RDGraphNode n) { m_root = n; }

void Graph::removeEdge(const RDGraphEdge* edge)
{
    auto it = std::find_if(m_edges.begin(), m_edges.end(), [edge](const RDGraphEdge& e) {
        return std::tie(edge->source, edge->target) ==
               std::tie(e.source, e.target);
    });

    if(it != m_edges.end()) m_edges.erase(it);
}

void Graph::removeNode(RDGraphNode n)
{
    auto it = std::find(m_nodes.begin(), m_nodes.end(), n);
    if(it == m_nodes.end()) return;

    m_nodes.erase(it);
    this->removeEdges(n);
}

void Graph::pushEdge(RDGraphNode source, RDGraphNode target)
{
    if(!this->containsEdge(source, target))
        m_edges.push_back({ source, target });
}

bool Graph::containsEdge(RDGraphNode source, RDGraphNode target) const { return this->edge(source, target); }

const RDGraphEdge* Graph::edge(RDGraphNode source, RDGraphNode target) const
{
    for(const RDGraphEdge& e : m_edges)
    {
        if((e.source == source) && (e.target == target))
            return &e;
    }

    return nullptr;
}

size_t Graph::outgoing(RDGraphNode n, const RDGraphEdge** edges) const
{
    m_outgoings.clear();

    for(size_t i = 0; i < m_edges.size(); i++)
    {
        const RDGraphEdge& e = m_edges[i];
        if(e.source != n) continue;

        m_outgoings.push_back(e);
    }

    if(edges) *edges = m_outgoings.data();
    return m_outgoings.size();
}

size_t Graph::incoming(RDGraphNode n, const RDGraphEdge** edges) const
{
    m_incomings.clear();

    for(size_t i = 0; i < m_edges.size(); i++)
    {
        const RDGraphEdge& e = m_edges[i];
        if(e.target != n) continue;

        m_incomings.push_back(e);
    }

    if(edges) *edges = m_incomings.data();
    return m_incomings.size();
}

size_t Graph::nodes(const RDGraphNode** nodes) const { if(nodes) *nodes = m_nodes.data(); return m_nodes.size(); }
size_t Graph::edges(const RDGraphEdge** edges) const { if(edges) *edges = m_edges.data(); return m_edges.size(); }
RDGraphNode Graph::root() const { return m_root; }
RDGraphNode Graph::pushNode() { RDGraphNode n = ++m_nodeid; m_nodes.push_back(n); return n; }

void Graph::removeEdges(RDGraphNode n)
{
    for(size_t i = 0; i < m_edges.size(); )
    {
        const RDGraphEdge& e = m_edges[i];

        if(e.source == n) this->removeEdge(&e);
        else i++;
    }
}
