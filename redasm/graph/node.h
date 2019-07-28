#pragma once

#include "../types/containers/list.h"

namespace REDasm {

typedef int Node;

class NodeListImpl;

class NodeList: public Object
{
    REDASM_OBJECT(NodeList)
    PIMPL_DECLARE_P(NodeList)
    PIMPL_DECLARE_PRIVATE(NodeList)
    DECLARE_SORT_FIND(Node)

    public:
        NodeList();
        size_t size() const;
        Node at(size_t idx) const;
        bool empty() const;
        void append(Node n);
        void erase(Node n);
        void clear();
};

} // namespace REDasm
