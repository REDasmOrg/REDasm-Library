#pragma once

#include "node.h"

namespace REDasm {

class Edge
{
    public:
        Edge();
        Edge(Node source, Node target);
        bool valid() const;
        const Node& opposite(Node n) const;
        bool operator ==(const Edge& e) const;
        bool operator !=(const Edge& e) const;

    public:
        Node source, target;
};

class EdgeListImpl;

class EdgeList: public Object
{
    REDASM_OBJECT(EdgeList)
    PIMPL_DECLARE_P(EdgeList)
    PIMPL_DECLARE_PRIVATE(EdgeList)
    DECLARE_SORT_FIND(Edge)

    public:
        EdgeList();
        size_t size() const;
        const Edge& at(size_t idx) const;
        bool empty() const;
        void append(const Edge& e);
        void erase(const Edge& e);
        void clear();
        const Edge& operator[](size_t idx) const;
};

} // namespace REDasm

namespace std {

template<> struct hash<REDasm::Edge> {
    size_t operator()(const REDasm::Edge& edge) const;
};

} // namespace std
