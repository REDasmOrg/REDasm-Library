#pragma once

#include <unordered_map>
#include <deque>
#include <rdapi/graph/layout.h>
#include "../../graph/styledgraph.h"

struct LLBlock;

struct LLPoint
{
    int row;   // point[0]
    int col;   // point[1]
    int index; // point[2]
};

struct LLEdge
{
    LLBlock *sourceblock, *targetblock;
    std::deque<LLPoint> points;
    int startindex = 0;
    GraphPoints routes, arrow;

    void addPoint(int row, int col, int index = 0) {
        LLPoint point = {row, col, 0};
        this->points.push_back(point);

        if(this->points.size() > 1)
            this->points[this->points.size() - 2].index = index;
    }
};

struct LLBlock
{
    LLBlock() { }
    LLBlock(const RDGraphNode& n, int w, int h) : node(n), width(w), height(h) { }

    RDGraphNode node;
    std::deque<LLEdge> edges;
    std::deque<RDGraphNode> incoming, newoutgoing;

    float x{0.0}, y{0.0};
    int width{0}, height{0};
    int col{0}, colcount{0};
    int row{0}, rowcount{0};
};

class LayeredLayout
{
    private:
        template<typename T> using Matrix = std::deque< std::deque<T> >;
        using EdgesVector = Matrix< std::deque<bool> >;

    public:
        LayeredLayout(StyledGraph* graph, rd_type type);
        bool execute();

    private: // Refactored functions
        void createBlocks();
        void makeAcyclic();
        void prepareEdgeRouting();
        void performEdgeRouting();
        void computeEdgeCount();
        void computeRowColumnSizes();
        void computeRowColumnPositions();
        void computeNodePositions();
        void precomputeEdgeCoordinates();

    private: // Algorithm functions
        LLEdge routeEdge(EdgesVector& horizedges, EdgesVector& vertedges, Matrix<bool>& edgevalid, LLBlock& start, LLBlock& end);
        int findHorizEdgeIndex(EdgesVector& edges, int row, int mincol, int maxcol) const;
        int findVertEdgeIndex(EdgesVector &edges, int col, int minrow, int maxrow) const;
        bool isEdgeMarked(EdgesVector& edges, int row, int col, int index) const;
        void markEdge(EdgesVector& edges, int row, int col, int index, bool used = true) const;
        void adjustGraphLayout(LLBlock& block, int col, int row);
        void computeLayout(LLBlock &block);

    private:
        template<typename T> static void removeFromDeque(std::deque<T>& v, T item) { v.erase(std::remove(v.begin(), v.end(), item), v.end()); }
        template<typename T> static void initDeque(std::deque<T>& v, size_t size, T value) { v.resize(size); for(size_t i = 0; i < size; i++) v[i] = value; }

    private:
        StyledGraph* m_graph;
        rd_type m_layouttype;

    private: // Layout fields
        std::unordered_map<RDGraphNode, LLBlock> m_blocks;
        std::deque<int> m_colx, m_rowy, m_coledgex, m_rowedgey, m_colwidth, m_rowheight, m_coledgecount, m_rowedgecount;
        std::deque<RDGraphNode> m_blockorder;
        EdgesVector m_horizedges, m_vertedges;
        Matrix<bool> m_edgevalid;
};

