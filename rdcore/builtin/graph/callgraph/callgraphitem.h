#pragma once

#include <rdapi/types.h>
#include <rdapi/graph/callgraph.h>
#include <deque>
#include "../../../rdil/expressions.h"

class CallGraphItem
{
    public:
        CallGraphItem(RDGraphNode n, rd_address address);
        RDGraphNode node() const;
        rd_address address() const;
        void addCall(const ILExpression* e);
        const ILExpression* call(size_t idx) const;
        size_t size() const;

    private:
        RDGraphNode m_node;
        rd_address m_address;
        std::deque<ILExpressionPtr> m_calls;
};

