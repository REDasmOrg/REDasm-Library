#pragma once

#include <algorithm>
#include <memory>
#include <rdapi/types.h>
#include "../../containers/sortedcontainer.h"
#include "../../builtin/graph/functiongraph.h"

class FunctionContainer: public SortedContainer<rd_address, std::equal_to<rd_address>, std::less<rd_address>, true>
{
    private:
        typedef SortedContainer<rd_address, std::equal_to<rd_address>, std::less<rd_address>, true> ContainerType;
        typedef std::unique_ptr<FunctionGraph> FunctionGraphPtr;

    public:
        FunctionContainer() = default;
        bool remove(rd_address address); // Hides base class method
        RDLocation findFunction(rd_address address) const;
        const FunctionBasicBlock* findBasicBlock(rd_address address) const;
        FunctionGraph* findGraph(rd_address address) const;
        void graph(FunctionGraph* g);
        void clearGraphs();

    private:
        std::unordered_map<rd_address, FunctionGraphPtr> m_graphs;
};

