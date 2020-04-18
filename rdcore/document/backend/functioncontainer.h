#pragma once

#include <algorithm>
#include <memory>
#include <rdapi/types.h>
#include "../../containers.h"
#include "../../builtin/graph/functiongraph.h"

class FunctionContainer: public SortedContainer<address_t, std::less<address_t>, std::equal_to<address_t>, true>
{
    private:
        typedef SortedContainer<address_t, std::less<address_t>, std::equal_to<address_t>, true> Type;
        typedef std::unique_ptr<FunctionGraph> FunctionGraphPtr;

    public:
        FunctionContainer() = default;
        void remove(address_t address); // Hides base class method
        RDLocation findFunction(address_t address) const;
        const FunctionBasicBlock* findBasicBlock(address_t address) const;
        FunctionGraph* findGraph(address_t address) const;
        void graph(FunctionGraph* g);
        void clearGraphs();

    private:
        std::unordered_map<address_t, FunctionGraphPtr> m_graphs;
};

