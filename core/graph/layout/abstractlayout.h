#pragma once

#include <functional>
#include "../graph.h"

#define LAYOUT_VSPACING  40
#define LAYOUT_HSPACING  40
#define EDGE_SPACING     20
#define EDGE_VSPACING    20
#define EDGE_OFFSET_STEP 10

namespace REDasm {
namespace Graphing {

class AbstractLayout
{
    public:
        AbstractLayout(Graph* graph);
        virtual bool execute() = 0;

    protected:
        int middleX(const Node& n) const;
        int middleY(const Node& n) const;
        int bottom(const Node& n) const;
        int right(const Node& n) const;

    protected:
        Graph* m_graph;
};

} // namespace Graphing
} // namespace REDasm
