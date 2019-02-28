#include "graph.h"
#include <algorithm>

namespace REDasm {
namespace Graphing {

Graph::Graph(): m_currentid(0) { }

void Graph::addNode(Node *n)
{
    if(m_graph.find(n->id) != m_graph.end())
        return;

    n->id = this->getId();
    this->emplace_back(n);
    m_graph[n->id] = AdjacencyList();
}

void Graph::addEdge(Node *from, Node *to)
{
    auto it = m_graph.find(from->id);

    if((it == m_graph.end()) || (m_graph.find(to->id) == m_graph.end()))
        return;

    auto& adjlist = it->second;

    if(this->edgeExists(adjlist, to))
        return;

    auto iit = std::lower_bound(adjlist.begin(), adjlist.end(), to, [=](const Node* n1, const Node* n2) -> bool {
        return this->compareEdge(n1, n2);
    });

    it->second.insert(iit, to);
}

const Graph::AdjacencyList &Graph::edges(const NodePtr &np) const { return this->edges(np.get()); }
const Graph::AdjacencyList &Graph::edges(Node *n) const { return m_graph.at(n->id); }
bool Graph::compareEdge(const Node *n1, const Node *n2) const { return n1->id < n2->id; }

bool Graph::edgeExists(const AdjacencyList &adjlist, Node *search) const
{
    for(Node* n: adjlist)
    {
        if(n == search)
            return true;
    }

    return false;
}

int Graph::getId() { return ++m_currentid; }

} // namespace Graphing
} // namespace REDasm
