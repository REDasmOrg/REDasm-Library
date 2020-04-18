#include "layeredlayout.h"
#include <unordered_set>
#include <queue>

#define LLAYOUT_PADDING      16
#define LLAYOUT_PADDING_DIV2 (LLAYOUT_PADDING / 2)
#define LLAYOUT_PADDING_DIV4 (LLAYOUT_PADDING / 4)
#define LLAYOUT_NODE_PADDING (2 * LLAYOUT_PADDING)

LayeredLayout::LayeredLayout(StyledGraph* graph, type_t type): m_graph(graph), m_layouttype(type) { }

bool LayeredLayout::execute()
{
    if(!m_graph->root()) return false;

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
    this->prepareEdgeRouting();                     // Prepare edge routing
    this->performEdgeRouting();                     // Perform edge routing
    this->computeEdgeCount();                       // Compute edge counts for each row and column
    this->computeRowColumnSizes();                  // Compute row and column sizes
    this->computeRowColumnPositions();              // Compute row and column positions
    this->computeNodePositions();                   // Compute node positions
    this->precomputeEdgeCoordinates();              // Precompute coordinates for edges
    return true;
}

void LayeredLayout::createBlocks()
{
    const RDGraphNode* nodes = nullptr;
    size_t c = m_graph->nodes(&nodes);

    for(size_t i = 0; i < c; i++)
    {
        RDGraphNode n = nodes[i];
        m_graph->height(n, m_graph->height(n) + LLAYOUT_NODE_PADDING); // Pad Node
        m_blocks[n] = LLBlock(n, m_graph->width(n), m_graph->height(n));
    }

    //Populate incoming lists
    for(const auto& item : m_blocks)
    {
        const LLBlock& block = item.second;

        const RDGraphEdge* edges = nullptr;
        c = m_graph->outgoing(item.first, &edges);

        for(size_t i = 0; i < c; i++)
        {
            const RDGraphEdge& e = edges[i];
            m_blocks[e.target].incoming.push_back(block.node);
        }
    }
}

void LayeredLayout::makeAcyclic()
{
    //Construct acyclic graph where each node is used as an edge exactly once
    std::unordered_set<RDGraphNode> visited;
    std::queue<RDGraphNode> queue;
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

            const RDGraphEdge* edges = nullptr;
            size_t c = m_graph->outgoing(block.node, &edges);

            for(size_t i = 0; i < c; i++)
            {
                const RDGraphEdge& e = edges[i];
                if(visited.count(e.target)) return;

                //If node has no more unseen incoming edges, add it to the graph layout now
                if(m_blocks[e.target].incoming.size() == 1)
                {
                    LayeredLayout::removeFromDeque(m_blocks[e.target].incoming, block.node);
                    block.newoutgoing.push_back(e.target);
                    queue.push(m_blocks[e.target].node);
                    visited.insert(e.target);
                    changed = true;
                }
                else
                    LayeredLayout::removeFromDeque(m_blocks[e.target].incoming, block.node);
            }
        }

        //No more nodes satisfy constraints, pick a node to continue constructing the graph
        int best = 0, bestparent, bestedges;

        for(auto& item : m_blocks)
        {
            LLBlock& block = item.second;
            if(!visited.count(block.node)) continue;

            const RDGraphEdge* edges = nullptr;
            size_t c = m_graph->outgoing(block.node, &edges);

            for(size_t i = 0; i < c; i++)
            {
                const RDGraphEdge& e = edges[i];
                if(visited.count(e.target)) return;

                if(!best || (m_blocks[e.target].incoming.size() < bestedges) || ((m_blocks[e.target].incoming.size() == bestedges) && (e.target < best))) {
                    best = e.target;
                    bestedges = static_cast<int>(m_blocks[e.target].incoming.size());
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

void LayeredLayout::prepareEdgeRouting()
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

void LayeredLayout::performEdgeRouting()
{
    for(const RDGraphNode& n : m_blockorder)
    {
        LLBlock& block = m_blocks[n];
        LLBlock& start = block;

        const RDGraphEdge* edges = nullptr;
        size_t c = m_graph->outgoing(block.node, &edges);

        for(size_t i = 0; i < c; i++)
        {
            const RDGraphEdge& e = edges[i];
            LLBlock& end = m_blocks[e.target];
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
        block.x = static_cast<float>((m_colx[block.col] + m_colwidth[block.col] + (LLAYOUT_PADDING_DIV2 / 2) * m_coledgecount[block.col + 1]) - (block.width / 2));

        if((block.x + block.width) > (m_colx[block.col] + m_colwidth[block.col] + m_colwidth[block.col + 1] + LLAYOUT_PADDING_DIV2 * m_coledgecount[block.col + 1]))
            block.x = static_cast<float>((m_colx[block.col] + m_colwidth[block.col] + m_colwidth[block.col + 1] + LLAYOUT_PADDING_DIV2 * m_coledgecount[block.col + 1]) - block.width);

        block.y = static_cast<float>(m_rowy[block.row] + LLAYOUT_PADDING);
        m_graph->x(block.node, static_cast<int>(block.x));
        m_graph->y(block.node, static_cast<int>(block.y));
    }
}

void LayeredLayout::precomputeEdgeCoordinates()
{
    for(auto& item : m_blocks)
    {
        LLBlock& block = item.second;

        for(LLEdge& edge : block.edges)
        {
            const LLPoint& start = edge.points.front();
            auto startcol = start.col;
            auto lastindex = edge.startindex;
            RDGraphPoint lastpt = { m_coledgex[startcol] + (LLAYOUT_PADDING_DIV2 * lastindex) + 4, m_graph->y(block.node) + m_graph->height(block.node) + 4 - LLAYOUT_NODE_PADDING };

            GraphPoints pts;
            pts.push_back(lastpt);

            for(size_t i = 0; i < edge.points.size(); i++)
            {
                auto end = edge.points[i];
                auto endrow = end.row;
                auto endcol = end.col;
                auto lastindex = end.index;
                RDGraphPoint newpt;

                if(startcol == endcol)
                    newpt = { lastpt.x, m_rowedgey[endrow] + (LLAYOUT_PADDING_DIV2 * lastindex) + 4 };
                else
                    newpt = { m_coledgex[endcol] + (LLAYOUT_PADDING_DIV2 * lastindex) + 4, lastpt.y };

                pts.push_back(newpt);
                lastpt = newpt;
                startcol = endcol;
            }

            RDGraphPoint newpt = { lastpt.x, m_graph->y(edge.targetblock->node) - 1 };
            pts.push_back(newpt);
            edge.routes = pts;

            pts = { };
            pts.push_back({ newpt.x - 3, newpt.y - 6 });
            pts.push_back({ newpt.x + 3, newpt.y - 6 });
            pts.push_back(newpt);
            edge.arrow = pts;

            const RDGraphEdge* e = m_graph->edge(edge.sourceblock->node, edge.targetblock->node);
            if(!e) continue;

            m_graph->routes(e, edge.routes.data(), edge.routes.size());
            m_graph->arrow(e, edge.arrow.data(), edge.arrow.size());
        }
    }
}

LLEdge LayeredLayout::routeEdge(LayeredLayout::EdgesVector& horizedges, LayeredLayout::EdgesVector& vertedges, Matrix<bool>& edgevalid, LLBlock& start, LLBlock& end)
{
    LLEdge edge;
    edge.sourceblock = &start;
    edge.targetblock = &end;

    //Find edge index for initial outgoing line
    int i = 0;

    while(true)
    {
        if(!this->isEdgeMarked(vertedges, start.row + 1, start.col + 1, i))
            break;
        i += 1;
    }

    this->markEdge(vertedges, start.row + 1, start.col + 1, i);
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
        auto checkColumn = [minrow, maxrow, &edgevalid](int column) -> bool {
            if(column < 0 || column >= static_cast<int>(edgevalid[minrow].size()))
                return false;

            for(int row = minrow; row < maxrow; row++) {
                if(!edgevalid[row][column])
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
                    if(checkColumn(col)) break;

                    col = start.col + 1 + ofs;
                    if(checkColumn(col)) break;
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

        int index = this->findHorizEdgeIndex(horizedges, start.row + 1, mincol, maxcol);
        edge.addPoint(start.row + 1, col, index);
        horiz = true;
    }

    if(end.row != (start.row + 1))
    {
        //Not in same row, need to generate a line for moving to the correct row
        if(col == (start.col + 1)) this->markEdge(vertedges, start.row + 1, start.col + 1, i, false);
        int index = this->findVertEdgeIndex(vertedges, col, minrow, maxrow);
        if(col == (start.col + 1)) edge.startindex = index;
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
        int index = this->findHorizEdgeIndex(horizedges, end.row, mincol, maxcol);
        edge.addPoint(end.row, end.col + 1, index);
        horiz = true;
    }

    //If last line was horizontal, choose the ending edge index for the incoming edge
    if(horiz)
    {
        int index = this->findVertEdgeIndex(vertedges, end.col + 1, end.row, end.row);
        edge.points[int(edge.points.size()) - 1].index = index;
    }

    return edge;
}

int LayeredLayout::findHorizEdgeIndex(LayeredLayout::EdgesVector& edges, int row, int mincol, int maxcol) const
{
    //Find a valid index
    int i = 0;

    while(true)
    {
        bool valid = true;

        for(int col = mincol; col < maxcol + 1; col++)
        {
            if(!this->isEdgeMarked(edges, row, col, i)) continue;
            valid = false;
            break;
        }

        if(valid) break;
        i++;
    }

    //Mark chosen index as used
    for(int col = mincol; col < maxcol + 1; col++) this->markEdge(edges, row, col, i);
    return i;
}

int LayeredLayout::findVertEdgeIndex(LayeredLayout::EdgesVector& edges, int col, int minrow, int maxrow) const
{
    //Find a valid index
    int i = 0;

    while(true)
    {
        bool valid = true;

        for(int row = minrow; row < maxrow + 1; row++)
        {
            if(!this->isEdgeMarked(edges, row, col, i)) continue;
            valid = false;
            break;
        }

        if(valid) break;
        i++;
    }

    //Mark chosen index as used
    for(int row = minrow; row < maxrow + 1; row++) this->markEdge(edges, row, col, i);
    return i;
}

bool LayeredLayout::isEdgeMarked(LayeredLayout::EdgesVector& edges, int row, int col, int index) const
{
    if(index >= static_cast<int>(edges[row][col].size())) return false;
    return edges[row][col][index];
}

void LayeredLayout::markEdge(LayeredLayout::EdgesVector& edges, int row, int col, int index, bool used) const
{
    while(static_cast<int>(edges[row][col].size()) <= index)
        edges[row][col].push_back(false);

    edges[row][col][index] = used;
}

void LayeredLayout::adjustGraphLayout(LLBlock& block, int col, int row)
{
    block.col += col;
    block.row += row;

    for(const RDGraphNode& n : block.newoutgoing)
        this->adjustGraphLayout(m_blocks[n], col, row);
}

void LayeredLayout::computeLayout(LLBlock& block)
{
    //Compute child node layouts and arrange them horizontally
    int col = 0;
    int rowcount = 1;
    int childcolumn = 0;
    bool singlechild = block.newoutgoing.size() == 1;

    for(size_t i = 0; i < block.newoutgoing.size(); i++)
    {
        const RDGraphNode& n = block.newoutgoing[i];
        this->computeLayout(m_blocks[n]);

        if((m_blocks[n].rowcount + 1) > rowcount)
            rowcount = m_blocks[n].rowcount + 1;

        childcolumn = m_blocks[n].col;
    }

    if(m_layouttype != RDLayeredLayoutType::LayeredLayoutType_Wide && block.newoutgoing.size() == 2)
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

        if(m_layouttype == RDLayeredLayoutType::LayeredLayoutType_Medium)
            block.col = (left.col + right.col) / 2;
        else
            block.col = singlechild ? childcolumn : (col - 2) / 2;
    }
    else
    {
        for(const RDGraphNode& n : block.newoutgoing)
        {
            this->adjustGraphLayout(m_blocks[n], col, 1);
            col += m_blocks[n].colcount;
        }

        if(col >= 2)
        {
            //Place this node centered over the child nodes
            block.col = singlechild ? childcolumn : (col - 2) / 2;
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
