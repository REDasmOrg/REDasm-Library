#pragma once

#include "../../graph/styledgraph.h"
#include "../../rdil/ilfunction.h"
#include <deque>

class Disassembler;

class RDILGraph: public StyledGraph
{
    public:
        RDILGraph(const Disassembler* disassembler);
        void build(rd_address address);

    private:
        RDGraphNode generate(const ILExpression* e, RDGraphNode parentnode);
        void generate(const ILFunction* il);

    private:
        const Disassembler* m_disassembler;
        std::deque<std::string> m_strings;
};

