#include "graph.h"
#include <algorithm>

namespace REDasm {
namespace Graphing {

bool Graph::containsEdge(const Node &source, const Node &target) const
{
    for(const Edge& e : m_edges)
    {
        if((e.source == source) && (e.target == target))
            return true;
    }

    return false;
}

void Graph::removeEdge(const Edge &edge) { m_edges.erase(std::find(m_edges.begin(), m_edges.end(), edge)); }

void Graph::removeNode(const Node &n)
{
    for(auto it = m_nodes.begin(); it != m_nodes.end(); it++)
    {
        if(*it != n)
            continue;

        m_nodes.erase(it);
        break;
    }

    this->removeEdges(n);
}

EdgeList Graph::outgoing(const Node &n) const
{
    EdgeList oe;

    for(auto it = m_edges.begin(); it != m_edges.end(); it++)
    {
        if(it->source != n)
            continue;

        oe.push_back(*it);
    }

    return oe;
}

EdgeList Graph::incoming(const Node &n) const
{
    EdgeList ie;

    for(auto it = m_edges.begin(); it != m_edges.end(); it++)
    {
        if(it->target != n)
            continue;

        ie.push_back(*it);
    }

    return ie;
}

Edge Graph::edge(const Node &source, const Node &target) const
{
    for(const Edge& e : m_edges)
    {
        if((e.source == source) && (e.target == target))
            return e;
    }

    return Edge();
}

void Graph::newEdge(const Node &source, const Node &target)
{
    if(this->containsEdge(source, target))
        return;

    m_edges.emplace_back(source, target);
}

Node Graph::newNode()
{
    Node n = ++m_nodeid;
    m_nodes.push_back(n);
    return n;
}

void Graph::removeEdges(const Node &n)
{
    auto it = m_edges.begin();

    while(it != m_edges.end())
    {
        if(it->source == n)
            it = m_edges.erase(it);
        else
            it++;
    }
}

} // namespace Graphing
} // namespace REDasm
