#pragma once

#include <redasm/graph/graph.h>
#include <unordered_map>

namespace REDasm {

class GraphImpl
{
    public:
        GraphImpl();
        bool empty() const;
        bool containsEdge(Node source, Node target) const;
        void removeEdge(const Edge &e);
        void removeNode(Node n);
        EdgeList outgoing(Node n) const;
        EdgeList incoming(Node n) const;
        const NodeList& nodes() const;
        const EdgeList& edges() const;
        Edge edge(Node source, Node target) const;
        void newEdge(Node source, Node target);
        void setRoot(Node n);
        Node root() const;
        Node newNode();

    public: // Styling
        int areaWidth() const;
        int areaHeight() const;
        void areaWidth(int w);
        void areaHeight(int h);
        int x(Node n) const;
        int y(Node n) const;
        int width(Node n) const;
        int height(Node n) const;
        void x(Node n, int px);
        void y(Node n, int py);
        void width(Node n, int w);
        void height(Node n, int h);
        const String& color(const Edge& e) const;
        const String& label(const Edge& e) const;
        const Polyline& routes(const Edge& e) const;
        const Polyline& arrow(const Edge& e) const;
        void color(const Edge& e, const String& c);
        void label(const Edge& e, const String& s);
        void routes(const Edge& e, const Polyline& p);
        void arrow(const Edge& e, const Polyline& p);

    public: // Data
        const Dictionary& data() const;
        Variant data(Node n) const;
        void setData(Node n, const Variant& v);

    private:
        void removeEdges(Node n);

    private:
        size_t m_nodeid;
        int m_areawidth, m_areaheight;
        std::unordered_map<Node, NodeAttributes> m_nodeattributes;
        std::unordered_map<Edge, EdgeAttributes> m_edgeattributes;
        Dictionary m_data;
        EdgeList m_edges;
        NodeList m_nodes;
        Node m_root;
};

} // namespace REDasm
