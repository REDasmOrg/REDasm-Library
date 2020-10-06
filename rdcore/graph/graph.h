#pragma once

#include <rdapi/graph/graph.h>
#include <string>
#include <vector>
#include "../object.h"

class Graph: public Object
{
    public:
        Graph(Context* ctx);
        void setRoot(RDGraphNode n);
        void removeEdge(const RDGraphEdge* e);
        void removeNode(RDGraphNode n);
        void pushEdge(RDGraphNode source, RDGraphNode target);
        RDGraphNode pushNode();
        std::string generateDOT() const;
        u32 hash() const;

    public:
        bool empty() const;
        bool containsEdge(RDGraphNode source, RDGraphNode target) const;
        const RDGraphEdge* edge(RDGraphNode source, RDGraphNode target) const;
        size_t outgoing(RDGraphNode n, const RDGraphEdge** edges) const;
        size_t incoming(RDGraphNode n, const RDGraphEdge** edges) const;
        size_t nodes(const RDGraphNode** nodes) const;
        size_t edges(const RDGraphEdge** edges) const;
        RDGraphNode root() const;

    protected:
        virtual std::string nodeLabel(RDGraphNode n) const;
        void removeOutgoingEdges(RDGraphNode n);
        void removeIncomingEdges(RDGraphNode n);
        void removeEdges(RDGraphNode n);

    protected:
        mutable std::vector<RDGraphEdge> m_incomings, m_outgoings;
        std::vector<RDGraphEdge> m_edges;
        std::vector<RDGraphNode> m_nodes;
        size_t m_nodeid{0};
        RDGraphNode m_root{0};
};
