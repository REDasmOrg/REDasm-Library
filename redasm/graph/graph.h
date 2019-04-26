#pragma once

#include <unordered_map>
#include <cstddef>
#include <string>
#include <deque>
#include <list>

namespace REDasm {
namespace Graphing {

typedef int Node;
struct Point { int x, y; };
typedef std::deque<Point> Polyline;

struct Edge {
    Edge(const Node& source, const Node& target): source(source), target(target) { }
    Edge(): source(0), target(0) { }
    Node source, target;

    inline bool valid() const { return (source != 0) && (target != 0); }
    inline const Node& opposite(const Node& n) const { return (n == source) ? target : source; }
    bool operator ==(const Edge& e) const { return (source == e.source) && (target == e.target); }
    bool operator !=(const Edge& e) const { return (source != e.source) || (target != e.target); }
};

typedef std::deque<Edge> EdgeList;
typedef std::deque<Node> NodeList;

struct NodeAttributes {
    NodeAttributes(): x(0), y(0), width(0), height(0) { }
    int x, y, width, height;
};

struct EdgeAttributes {
    std::string label, color;
    Polyline routes, arrow;
};

} // namespace Graphing
} // namespace REDasm

namespace std {
template<> struct hash<REDasm::Graphing::Edge> {
    size_t operator()(const REDasm::Graphing::Edge& edge) const {
        return edge.source ^ edge.target;
    }
};
} // namespace std

namespace REDasm {
namespace Graphing {

class Graph
{
    public:
        Graph(): m_nodeid(0), m_areawidth(0), m_areaheight(0), m_root(0) { }
        bool empty() const { return m_nodes.empty(); }
        bool containsEdge(const Node& source, const Node& target) const;
        void removeEdge(const Edge& edge);
        void removeNode(const Node& n);
        EdgeList outgoing(const Node& n) const;
        EdgeList incoming(const Node& n) const;
        const NodeList& nodes() const { return m_nodes; }
        const EdgeList& edges() const { return m_edges; }
        Edge edge(const Node& source, const Node& target) const;
        void newEdge(const Node& source, const Node& target);
        Node newNode();
        Node root() { return m_root; }

    private:
        void removeEdges(const Node& n);

    public: // Styling
        int areaWidth() const { return m_areawidth; }
        void areaWidth(int w) { m_areawidth = w; }
        int areaHeight() const { return m_areaheight; }
        void areaHeight(int h) { m_areaheight = h; }
        int x(const Node& n) const { return m_nodeattributes.at(n).x; }
        int y(const Node& n) const { return m_nodeattributes.at(n).y; }
        void x(const Node& n, int px) { m_nodeattributes.at(n).x = px; }
        void y(const Node& n, int py) { m_nodeattributes.at(n).y = py; }
        int width(const Node& n) const { return m_nodeattributes.at(n).width; }
        int height(const Node& n) const { return m_nodeattributes.at(n).height; }
        void width(const Node& n, int w) { m_nodeattributes[n].width = w; }
        void height(const Node& n, int h) { m_nodeattributes[n].height = h; }
        const std::string& color(const Edge& e) const { return m_edgeattributes.at(e).color; }
        void color(const Edge& e, const std::string& c) { m_edgeattributes[e].color = c; }
        const std::string& label(const Edge& e) const { return m_edgeattributes.at(e).label; }
        void label(const Edge& e, const std::string& s) { m_edgeattributes[e].label = s; }
        const Polyline& routes(const Edge& e) const { return m_edgeattributes.at(e).routes; }
        void routes(const Edge& e, const Polyline& p) { m_edgeattributes.at(e).routes = p; }
        const Polyline& arrow(const Edge& e) const { return m_edgeattributes.at(e).arrow; }
        void arrow(const Edge& e, const Polyline& p) { m_edgeattributes.at(e).arrow = p; }

    protected:
        size_t m_nodeid;
        int m_areawidth, m_areaheight;
        std::unordered_map<Node, NodeAttributes> m_nodeattributes;
        std::unordered_map<Edge, EdgeAttributes> m_edgeattributes;
        EdgeList m_edges;
        NodeList m_nodes;
        Node m_root;
};

template<typename T> class GraphT: public Graph
{
    public:
        GraphT(): Graph() { }
        const T* data(const Node& n) const;

    protected:
        std::unordered_map<Node, T> m_data;
};

template<typename T> const T* GraphT<T>::data(const Node &n) const
{
    auto it = m_data.find(n);

    if(it != m_data.end())
        return &it->second;

    return nullptr;
}

} // namespace Graphing
} // namespace REDasm
