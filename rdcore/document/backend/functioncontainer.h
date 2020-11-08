#pragma once

#include <algorithm>
#include <memory>
#include <rdapi/types.h>
#include "../../containers/treecontainer.h"
#include "../../builtin/graph/functiongraph.h"

typedef std::unique_ptr<FunctionGraph> FunctionGraphPtr;

class FunctionContainer: public TreeContainer<rd_address>
{
    private:
        typedef SortedContainer<rd_address, std::equal_to<rd_address>, std::less<rd_address>, true> ContainerType;

    public:
        FunctionContainer() = default;
        bool remove(const rd_address& address) override;
        RDLocation findFunction(rd_address address) const;
        const FunctionBasicBlock* findBasicBlock(rd_address address) const;
        FunctionGraph* findGraph(rd_address address) const;
        void graph(FunctionGraph* g);
        void clearGraphs();

    private:
        std::unordered_map<rd_address, FunctionGraphPtr> m_graphs;
};

