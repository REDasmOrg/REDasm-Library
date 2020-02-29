#pragma once

#include "../types/containers/dictionary.h"
#include "../types/containers/list.h"
#include "../types/containers/list.h"
#include "../types/string.h"
#include "../macros.h"
#include "polyline.h"
#include "edge.h"
#include "node.h"

namespace REDasm {

class GraphImpl;

struct NodeAttributes
{
    NodeAttributes();
    int x, y, width, height;
};

struct EdgeAttributes
{
    String label, color;
    Polyline routes, arrow;
};

class LIBREDASM_API Graph: public Object
{
    REDASM_OBJECT(Graph)
    PIMPL_DECLARE_P(Graph)
    PIMPL_DECLARE_PRIVATE(Graph)

    protected:
        Graph(GraphImpl *p);

    public:
        Graph();
        bool empty() const;
        bool containsEdge(Node source, Node target) const;
        void removeEdge(const Edge& e);
        void removeNode(Node n);
        EdgeList outgoing(Node n) const;
        EdgeList incoming(Node n) const;
        const NodeList& nodes() const;
        const EdgeList& edges() const;
        Edge edge(Node source, Node target) const;
        void newEdge(Node source, Node target);
        Node root() const;
        Node newNode();

    public: // Styling
        int areaWidth() const;
        int areaHeight() const;
        void areaWidth(int w);
        void areaHeight(int h);
        int x(Node n) const;
        int y(Node n) const;
        void x(Node n, int px);
        void y(Node n, int py);
        int width(Node n) const;
        int height(Node n) const;
        void width(Node n, int w);
        void height(Node n, int h);
        const String& color(const Edge& e) const;
        const String& label(const Edge& e) const;
        const Polyline& routes(const Edge &e) const;
        const Polyline& arrow(const Edge &e) const;
        void color(const Edge& e, const String& c);
        void label(const Edge& e, const String& s);
        void routes(const Edge& e, const Polyline& pl);
        void arrow(const Edge& e, const Polyline& pl);

    public: // Data
        const Dictionary& data() const;
        Variant data(Node n) const;

    protected:
        void setData(Node n, const Variant& v);
        void setRoot(Node n);
};

} // namespace REDasm
