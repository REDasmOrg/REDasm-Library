#pragma once

#include <redasm/graph/functiongraph.h>
#include <redasm/pimpl.h>

namespace REDasm {
namespace Graphing {

class FunctionBasicBlockImpl
{
    PIMPL_DECLARE_Q(FunctionBasicBlock)
    PIMPL_DECLARE_PUBLIC(FunctionBasicBlock)

    public:
        FunctionBasicBlockImpl();
        FunctionBasicBlockImpl(size_t startidx);

    private:
        Node m_node;
        size_t m_startidx, m_endidx; // [startidx, endidx]
        std::unordered_map<Node, String> m_styles;
};

class FunctionGraphImpl
{
    PIMPL_DECLARE_Q(FunctionGraph)
    PIMPL_DECLARE_PUBLIC(FunctionGraph)

    public:
        FunctionGraphImpl();
};

} // namespace Graphing
} // namespace REDasm
