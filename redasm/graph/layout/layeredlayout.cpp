#include "layeredlayout.h"
#include <unordered_set>
#include <queue>
#include <iostream>

#define LLAYOUT_PADDING      16
#define LLAYOUT_PADDING_DIV2 (LLAYOUT_PADDING / 2)
#define LLAYOUT_PADDING_DIV4 (LLAYOUT_PADDING / 4)
#define LLAYOUT_NODE_PADDING (2 * LLAYOUT_PADDING)

namespace REDasm {
namespace Graphing {

LayeredLayout::LayeredLayout(Graph *graph): AbstractLayout(graph), m_layouttype(LayoutType::Medium) { }
void LayeredLayout::setLayoutType(int lt) { m_layouttype = lt; }

bool LayeredLayout::execute()
{
    if(!m_graph->root())
        return false;

    m_blocks.clear();
    m_colx.clear();
    m_rowy.clear();
    m_rowedgey.clear();
    m_colwidth.clear();
    m_rowheight.clear();
    m_coledgecount.clear();
    m_rowedgey.clear();
    m_blockorder.clear();
    m_horizedges.clear();
    m_vertedges.clear();
    m_edgevalid.clear();

    this->createBlocks();                           // Create render nodes
    this->makeAcyclic();                            // Construct acyclic graph where each node is used as an edge exactly once
    this->computeLayout(m_blocks[m_graph->root()]); // Compute graph layout from bottom up
    this->prepareRouting();                         // Prepare edge routing
    this->performRouting();                         // Perform edge routing
    this->computeEdgeCount();                       // Compute edge counts for each row and column
    this->computeRowColumnSizes();                  // Compute row and column sizes
    this->computeRowColumnPositions();              // Compute row and column positions
    this->computeNodePositions();                   // Compute node positions
    this->precomputeEdgeCoordinates();              // Precompute coordinates for edges
    return true;
}

void LayeredLayout::createBlocks()
{
    for(const Node& n : m_graph->nodes())
    {
        m_graph->height(n, m_graph->height(n) + LLAYOUT_NODE_PADDING); // Pad Node
        m_blocks[n] = LLBlock(n, m_graph->width(n), m_graph->height(n));
    }

    //Populate incoming lists
    for(const auto& item : m_blocks)
    {
        const LLBlock& block = item.second;

        for(const Edge& edge : m_graph->outgoing(item.first))
            m_blocks[edge.target].incoming.push_back(block.node);
    }
}

void LayeredLayout::makeAcyclic()
{
    //Construct acyclic graph where each node is used as an edge exactly once
    std::unordered_set<Node> visited;
    std::queue<Node> queue;
    bool changed = true;

    visited.insert(m_graph->root());
    queue.push(m_blocks[m_graph->root()].node);

    while(changed)
    {
        changed = false;

        //First pick nodes that have single entry points
        while(!queue.empty())
        {
            LLBlock& block = m_blocks[queue.front()];
            queue.pop();
            m_blockorder.push_back(block.node);

            for(const Edge& edge : m_graph->outgoing(block.node))
            {
                if(visited.count(edge.target))
                    continue;

                //If node has no more unseen incoming edges, add it to the graph layout now
                if(m_blocks[edge.target].incoming.size() == 1)
                {
                    LayeredLayout::removeFromDeque(m_blocks[edge.target].incoming, block.node);
                    block.newoutgoing.push_back(edge.target);
                    queue.push(m_blocks[edge.target].node);
                    visited.insert(edge.target);
                    changed = true;
                }
                else
                    LayeredLayout::removeFromDeque(m_blocks[edge.target].incoming, block.node);
            }
        }

        //No more nodes satisfy constraints, pick a node to continue constructing the graph
        int best = 0, bestparent, bestedges;

        for(auto& item : m_blocks)
        {
            LLBlock& block = item.second;

            if(!visited.count(block.node))
                continue;

            for(const Edge& edge : m_graph->outgoing(block.node))
            {
                if(visited.count(edge.target))
                    continue;

                if(!best || (m_blocks[edge.target].incoming.size() < bestedges) || ((m_blocks[edge.target].incoming.size() == bestedges) && (edge.target < best)))
                {
                    best = edge.target;
                    bestedges = m_blocks[edge.target].incoming.size();
                    bestparent = block.node;
                }
            }
        }

        if(best)
        {
            LLBlock& bestparentb = m_blocks[bestparent];
            LayeredLayout::removeFromDeque(m_blocks[best].incoming, bestparentb.node);
            bestparentb.newoutgoing.push_back(best);
            visited.insert(best);
            queue.push(best);
            changed = true;
        }
    }
}

void LayeredLayout::prepareRouting()
{
    m_horizedges.resize(m_blocks[m_graph->root()].rowcount + 1);
    m_vertedges.resize(m_blocks[m_graph->root()].rowcount + 1);
    m_edgevalid.resize(m_blocks[m_graph->root()].rowcount + 1);

    for(size_t row = 0; row < m_blocks[m_graph->root()].rowcount + 1; row++)
    {
        m_horizedges[row].resize(m_blocks[m_graph->root()].colcount + 1);
        m_vertedges[row].resize(m_blocks[m_graph->root()].colcount + 1);
        LayeredLayout::initDeque(m_edgevalid[row], m_blocks[m_graph->root()].colcount + 1, true);

        for(int col = 0; col < m_blocks[m_graph->root()].colcount + 1; col++)
        {
            m_horizedges[row][col].clear();
            m_vertedges[row][col].clear();
        }
    }

    for(const auto& item : m_blocks)
    {
        const LLBlock& block = item.second;
        m_edgevalid[block.row][block.col + 1] = false;
    }
}

void LayeredLayout::performRouting()
{
    for(const Node& n : m_blockorder)
    {
        LLBlock& block = m_blocks[n];
        LLBlock& start = block;

        for(const Edge& edge : m_graph->outgoing(block.node))
        {
            LLBlock& end = m_blocks[edge.target];
            start.edges.push_back(this->routeEdge(m_horizedges, m_vertedges, m_edgevalid, start, end));
        }
    }
}

void LayeredLayout::computeEdgeCount()
{
    LayeredLayout::initDeque(m_coledgecount, m_blocks[m_graph->root()].colcount + 1, 0);
    LayeredLayout::initDeque(m_rowedgecount, m_blocks[m_graph->root()].rowcount + 1, 0);

    for(int row = 0; row < m_blocks[m_graph->root()].rowcount + 1; row++)
    {
        for(int col = 0; col < m_blocks[m_graph->root()].colcount + 1; col++)
        {
            if(static_cast<int>(m_horizedges[row][col].size()) > m_rowedgecount[row])
                m_rowedgecount[row] = static_cast<int>(m_horizedges[row][col].size());

            if(static_cast<int>(m_vertedges[row][col].size()) > m_coledgecount[col])
                m_coledgecount[col] = static_cast<int>(m_vertedges[row][col].size());
        }
    }
}

void LayeredLayout::computeRowColumnSizes()
{
    LayeredLayout::initDeque(m_colwidth, m_blocks[m_graph->root()].colcount + 1, 0);
    LayeredLayout::initDeque(m_rowheight, m_blocks[m_graph->root()].rowcount + 1, 0);

    for(const auto & item : m_blocks)
    {
        const LLBlock& block = item.second;

        if((block.width / 2) > m_colwidth[block.col])
            m_colwidth[block.col] = block.width / 2;

        if((block.width / 2) > m_colwidth[block.col + 1])
            m_colwidth[block.col + 1] = block.width / 2;

        if(block.height > m_rowheight[block.row])
            m_rowheight[block.row] = block.height;
    }
}

void LayeredLayout::computeRowColumnPositions()
{
    LayeredLayout::initDeque(m_colx, m_blocks[m_graph->root()].colcount, 0);
    LayeredLayout::initDeque(m_rowy, m_blocks[m_graph->root()].rowcount, 0);
    LayeredLayout::initDeque(m_coledgex, m_blocks[m_graph->root()].colcount + 1, 0);
    LayeredLayout::initDeque(m_rowedgey, m_blocks[m_graph->root()].rowcount + 1, 0);

    int x = LLAYOUT_PADDING;

    for(int i = 0; i < m_blocks[m_graph->root()].colcount; i++)
    {
        m_coledgex[i] = x;
        x += LLAYOUT_PADDING_DIV2 * m_coledgecount[i];
        m_colx[i] = x;
        x += m_colwidth[i];
    }

    int y = LLAYOUT_PADDING;

    for(int i = 0; i < m_blocks[m_graph->root()].rowcount; i++)
    {
        m_rowedgey[i] = y;
        y += LLAYOUT_PADDING_DIV2 * m_rowedgecount[i];
        m_rowy[i] = y;
        y += m_rowheight[i];
    }

    m_coledgex[m_blocks[m_graph->root()].colcount] = x;
    m_rowedgey[m_blocks[m_graph->root()].rowcount] = y;
    m_graph->areaWidth(x + LLAYOUT_PADDING + (LLAYOUT_PADDING_DIV2 * m_coledgecount[m_blocks[m_graph->root()].colcount]));
    m_graph->areaHeight(y + LLAYOUT_PADDING + (LLAYOUT_PADDING_DIV2 * m_rowedgecount[m_blocks[m_graph->root()].rowcount]));
}

void LayeredLayout::computeNodePositions()
{
    for(auto& item : m_blocks)
    {
        LLBlock& block = item.second;
        block.x = (m_colx[block.col] + m_colwidth[block.col] + (LLAYOUT_PADDING_DIV2 / 2) * m_coledgecount[block.col + 1]) - (block.width / 2);

        if((block.x + block.width) > (m_colx[block.col] + m_colwidth[block.col] + m_colwidth[block.col + 1] + LLAYOUT_PADDING_DIV2 * m_coledgecount[block.col + 1]))
            block.x = (m_colx[block.col] + m_colwidth[block.col] + m_colwidth[block.col + 1] + LLAYOUT_PADDING_DIV2 * m_coledgecount[block.col + 1]) - block.width;

        block.y = m_rowy[block.row] + LLAYOUT_PADDING;
        m_graph->x(block.node, block.x);
        m_graph->y(block.node, block.y);
    }
}

void LayeredLayout::precomputeEdgeCoordinates()
{
    for(auto& item : m_blocks)
    {
        LLBlock& block = item.second;

        for(LLEdge& edge : block.edges)
        {
            auto start = edge.points[0];
            auto startcol = start.col;
            auto lastindex = edge.startindex;
            Point lastpt = { m_coledgex[startcol] + (LLAYOUT_PADDING_DIV2 * lastindex) + 4, m_graph->y(block.node) + m_graph->height(block.node) + 4 - LLAYOUT_NODE_PADDING };

            Polyline pts;
            pts.push_back(lastpt);

            for(size_t i = 0; i < edge.points.size(); i++)
            {
                auto end = edge.points[i];
                auto endrow = end.row;
                auto endcol = end.col;
                auto lastindex = end.index;
                Point newpt;

                if(startcol == endcol)
                    newpt = { lastpt.x, m_rowedgey[endrow] + (LLAYOUT_PADDING_DIV2 * lastindex) + 4 };
                else
                    newpt = { m_coledgex[endcol] + (LLAYOUT_PADDING_DIV2 * lastindex) + 4, lastpt.y };

                pts.push_back(newpt);
                lastpt = newpt;
                startcol = endcol;
            }

            Point newpt = { lastpt.x, m_graph->y(edge.targetblock->node) - 1 };
            pts.push_back(newpt);
            edge.routes = pts;

            pts.clear();
            pts.push_back({ newpt.x - 3, newpt.y - 6 });
            pts.push_back({ newpt.x + 3, newpt.y - 6 });
            pts.push_back(newpt);
            edge.arrow = pts;

            Edge e = m_graph->edge(edge.sourceblock->node, edge.targetblock->node);

            if(!e.valid())
                continue;

            m_graph->routes(e, edge.routes);
            m_graph->arrow(e, edge.arrow);
        }
    }
}

LLEdge LayeredLayout::routeEdge(LayeredLayout::EdgesVector &horiz_edges, LayeredLayout::EdgesVector &vert_edges, Matrix<bool> &edge_valid, LLBlock &start, LLBlock &end)
{
    LLEdge edge;
    edge.sourceblock = &start;
    edge.targetblock = &end;

    //Find edge index for initial outgoing line
    int i = 0;

    while(true)
    {
        if(!this->isEdgeMarked(vert_edges, start.row + 1, start.col + 1, i))
            break;
        i += 1;
    }

    this->markEdge(vert_edges, start.row + 1, start.col + 1, i);
    edge.addPoint(start.row + 1, start.col + 1);
    edge.startindex = i;
    bool horiz = false;

    //Find valid column for moving vertically to the target node
    int minrow, maxrow;

    if(end.row < (start.row + 1))
    {
        minrow = end.row;
        maxrow = start.row + 1;
    }
    else
    {
        minrow = start.row + 1;
        maxrow = end.row;
    }

    int col = start.col + 1;

    if(minrow != maxrow)
    {
        auto checkColumn = [minrow, maxrow, &edge_valid](int column) -> bool {
            if(column < 0 || column >= static_cast<int>(edge_valid[minrow].size()))
                return false;

            for(int row = minrow; row < maxrow; row++) {
                if(!edge_valid[row][column])
                    return false;
            }

            return true;
        };

        if(!checkColumn(col))
        {
            if(!checkColumn(end.col + 1))
            {
                int ofs = 0;

                while(true)
                {
                    col = start.col + 1 - ofs;

                    if(checkColumn(col))
                        break;

                    col = start.col + 1 + ofs;

                    if(checkColumn(col))
                        break;

                    ofs += 1;
                }
            }
            else
                col = end.col + 1;
        }
    }

    if(col != (start.col + 1))
    {
        //Not in same column, need to generate a line for moving to the correct column
        int mincol, maxcol;

        if(col < (start.col + 1))
        {
            mincol = col;
            maxcol = start.col + 1;
        }
        else
        {
            mincol = start.col + 1;
            maxcol = col;
        }

        int index = this->findHorizEdgeIndex(horiz_edges, start.row + 1, mincol, maxcol);
        edge.addPoint(start.row + 1, col, index);
        horiz = true;
    }

    if(end.row != (start.row + 1))
    {
        //Not in same row, need to generate a line for moving to the correct row
        if(col == (start.col + 1))
            this->markEdge(vert_edges, start.row + 1, start.col + 1, i, false);
        int index = this->findVertEdgeIndex(vert_edges, col, minrow, maxrow);
        if(col == (start.col + 1))
            edge.startindex = index;
        edge.addPoint(end.row, col, index);
        horiz = false;
    }

    if(col != (end.col + 1))
    {
        //Not in ending column, need to generate a line for moving to the correct column
        int mincol, maxcol;
        if(col < (end.col + 1))
        {
            mincol = col;
            maxcol = end.col + 1;
        }
        else
        {
            mincol = end.col + 1;
            maxcol = col;
        }
        int index = this->findHorizEdgeIndex(horiz_edges, end.row, mincol, maxcol);
        edge.addPoint(end.row, end.col + 1, index);
        horiz = true;
    }

    //If last line was horizontal, choose the ending edge index for the incoming edge
    if(horiz)
    {
        int index = this->findVertEdgeIndex(vert_edges, end.col + 1, end.row, end.row);
        edge.points[int(edge.points.size()) - 1].index = index;
    }

    return edge;
}

int LayeredLayout::findHorizEdgeIndex(LayeredLayout::EdgesVector &edges, int row, int mincol, int maxcol) const
{
    //Find a valid index
    int i = 0;

    while(true)
    {
        bool valid = true;

        for(int col = mincol; col < maxcol + 1; col++)
        {
            if(!isEdgeMarked(edges, row, col, i))
                continue;

            valid = false;
            break;
        }

        if(valid)
            break;

        i++;
    }

    //Mark chosen index as used
    for(int col = mincol; col < maxcol + 1; col++)
        this->markEdge(edges, row, col, i);

    return i;
}

int LayeredLayout::findVertEdgeIndex(LayeredLayout::EdgesVector &edges, int col, int minrow, int maxrow) const
{
    //Find a valid index
    int i = 0;

    while(true)
    {
        bool valid = true;

        for(int row = minrow; row < maxrow + 1; row++)
        {
            if(!isEdgeMarked(edges, row, col, i))
                continue;

            valid = false;
            break;
        }

        if(valid)
            break;

        i++;
    }

    //Mark chosen index as used
    for(int row = minrow; row < maxrow + 1; row++)
        this->markEdge(edges, row, col, i);

    return i;
}

bool LayeredLayout::isEdgeMarked(LayeredLayout::EdgesVector &edges, int row, int col, int index) const
{
    if(index >= static_cast<int>(edges[row][col].size()))
        return false;

    return edges[row][col][index];
}

void LayeredLayout::markEdge(LayeredLayout::EdgesVector &edges, int row, int col, int index, bool used) const
{
    while(static_cast<int>(edges[row][col].size()) <= index)
        edges[row][col].push_back(false);

    edges[row][col][index] = used;
}

void LayeredLayout::adjustGraphLayout(LLBlock &block, int col, int row)
{
    block.col += col;
    block.row += row;

    for(const Node& n : block.newoutgoing)
        this->adjustGraphLayout(m_blocks[n], col, row);
}

void LayeredLayout::computeLayout(LLBlock &block)
{
    //Compute child node layouts and arrange them horizontally
    int col = 0;
    int rowcount = 1;
    int childColumn = 0;
    bool singlechild = block.newoutgoing.size() == 1;

    for(size_t i = 0; i < block.newoutgoing.size(); i++)
    {
        const Node& n = block.newoutgoing[i];
        this->computeLayout(m_blocks[n]);

        if((m_blocks[n].rowcount + 1) > rowcount)
            rowcount = m_blocks[n].rowcount + 1;

        childColumn = m_blocks[n].col;
    }

    if(m_layouttype != LayoutType::Wide && block.newoutgoing.size() == 2)
    {
        LLBlock& left = m_blocks[block.newoutgoing[0]];
        LLBlock& right = m_blocks[block.newoutgoing[1]];

        if(left.newoutgoing.size() == 0)
        {
            left.col = right.col - 2;
            int add = left.col < 0 ? - left.col : 0;
            this->adjustGraphLayout(right, add, 1);
            this->adjustGraphLayout(left, add, 1);
            col = right.colcount + add;
        }
        else if(right.newoutgoing.size() == 0)
        {
            this->adjustGraphLayout(left, 0, 1);
            this->adjustGraphLayout(right, left.col + 2, 1);
            col = std::max(left.colcount, right.col + 2);
        }
        else
        {
            this->adjustGraphLayout(left, 0, 1);
            this->adjustGraphLayout(right, left.colcount, 1);
            col = left.colcount + right.colcount;
        }

        block.colcount = std::max(2, col);

        if(m_layouttype == LayoutType::Medium)
            block.col = (left.col + right.col) / 2;
        else
            block.col = singlechild ? childColumn : (col - 2) / 2;
    }
    else
    {
        for(const Node& n : block.newoutgoing)
        {
            this->adjustGraphLayout(m_blocks[n], col, 1);
            col += m_blocks[n].colcount;
        }

        if(col >= 2)
        {
            //Place this node centered over the child nodes
            block.col = singlechild ? childColumn : (col - 2) / 2;
            block.colcount = col;
        }
        else
        {
            //No child nodes, set single node's width (nodes are 2 columns wide to allow
            //centering over a branch)
            block.col = 0;
            block.colcount = 2;
        }
    }

    block.row = 0;
    block.rowcount = rowcount;
}

} // namespace Graphing
} // namespace REDasm
