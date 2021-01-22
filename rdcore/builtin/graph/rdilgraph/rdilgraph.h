#pragma once

#include "../../../graph/styledgraph.h"
#include "../../../rdil/ilfunction.h"
#include <deque>

class RDILGraph: public StyledGraph
{
    public:
        RDILGraph(Context* ctx);
        void build(rd_address address);

    private:
        RDGraphNode generate(const ILExpression* e, RDGraphNode parentnode);
        void generate(const ILFunction* il);

    private:
        std::deque<std::string> m_strings;
};

