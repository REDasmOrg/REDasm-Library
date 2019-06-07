#include "layeredlayout.h"
#include <impl/graph/layout/layeredlayout_impl.h>
#include <unordered_set>
#include <queue>
#include <iostream>

namespace REDasm {
namespace Graphing {

LayeredLayout::LayeredLayout(Graph *graph): AbstractLayout(graph), m_pimpl_p(new LayeredLayoutImpl(this))  { }
void LayeredLayout::setLayoutType(LayeredLayoutType lt) { PIMPL_P(LayeredLayout); p->m_layouttype = lt; }

bool LayeredLayout::execute()
{
    if(!m_graph->root())
        return false;

    PIMPL_P(LayeredLayout);

    p->m_blocks.clear();
    p->m_colx.clear();
    p->m_rowy.clear();
    p->m_rowedgey.clear();
    p->m_colwidth.clear();
    p->m_rowheight.clear();
    p->m_coledgecount.clear();
    p->m_rowedgey.clear();
    p->m_blockorder.clear();
    p->m_horizedges.clear();
    p->m_vertedges.clear();
    p->m_edgevalid.clear();

    p->createBlocks();                           // Create render nodes
    p->makeAcyclic();                            // Construct acyclic graph where each node is used as an edge exactly once
    p->computeLayout(p->m_blocks[m_graph->root()]); // Compute graph layout from bottom up
    p->prepareEdgeRouting();                     // Prepare edge routing
    p->performEdgeRouting();                     // Perform edge routing
    p->computeEdgeCount();                       // Compute edge counts for each row and column
    p->computeRowColumnSizes();                  // Compute row and column sizes
    p->computeRowColumnPositions();              // Compute row and column positions
    p->computeNodePositions();                   // Compute node positions
    p->precomputeEdgeCoordinates();              // Precompute coordinates for edges
    return true;
}

} // namespace Graphing
} // namespace REDasm
