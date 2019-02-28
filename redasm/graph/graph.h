#ifndef GRAPH_H
#define GRAPH_H

#include <unordered_map>
#include <memory>
#include <list>

namespace REDasm {
namespace Graphing {

struct Node { int id; };

typedef std::unique_ptr<Node> NodePtr;

class Graph: protected std::list<NodePtr>
{
    public:
        typedef std::list<Node*> AdjacencyList;

    private:
        typedef std::list<NodePtr> Type;

    public:
        using Type::begin;
        using Type::end;
        using Type::size;

    public:
        Graph();
        void addNode(Node* n);
        void addEdge(Node* from, Node* to);
        const AdjacencyList& edges(const NodePtr& np) const;
        const AdjacencyList& edges(Node *n) const;

    protected:
        virtual bool compareEdge(const Node *n1, const Node *n2) const;

    private:
        bool edgeExists(const AdjacencyList& adjlist, Node* search) const;
        int getId();

    private:
        std::unordered_map<int, AdjacencyList> m_graph;
        int m_currentid;
};

} // namespace Graphing
} // namespace REDasm

#endif // GRAPH_H
