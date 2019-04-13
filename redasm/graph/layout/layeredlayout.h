#ifndef LAYEREDLAYOUT_H
#define LAYEREDLAYOUT_H

// Sugiyama Layout Theory
// - https://drive.google.com/file/d/1uAAch1SxLLVBJ53ZX-zX4AnwzwhcXcEM/view
// - http://publications.lib.chalmers.se/records/fulltext/161388.pdf
// - http://www.graphviz.org/Documentation/TSE93.pdf
// - https://blog.disy.net/sugiyama-method

// Layout based on x64dbg's DisassemblerGraphView
// - https://github.com/x64dbg/x64dbg/blob/development/src/gui/Src/Gui/DisassemblerGraphView.h
// - https://github.com/x64dbg/x64dbg/blob/development/src/gui/Src/Gui/DisassemblerGraphView.cpp

#include <unordered_map>
#include <algorithm>
#include <deque>
#include "abstractlayout.h"

namespace REDasm {
namespace Graphing {

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

    Polyline routes;
    Polyline arrow;

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
    LLBlock(const Node& node, int w, int h) : node(node), width(w), height(h) { }

    Node node;
    std::deque<LLEdge> edges;
    std::deque<Node> incoming;
    std::deque<Node> newoutgoing;

    float x = 0.0, y = 0.0;
    int width = 0, height = 0;
    int col = 0, colcount = 0;
    int row = 0, rowcount = 0;
};

class LayeredLayout: public AbstractLayout
{
    public:
        enum LayoutType { Wide, Medium, Narrow };

    private:
        template<typename T> using Matrix = std::deque< std::deque<T> >;
        using EdgesVector = Matrix< std::deque<bool> >;

    public:
        LayeredLayout(Graph* graph);
        void setLayoutType(int lt);
        virtual bool execute();

    private: // Refactored functions
        void createBlocks();
        void makeAcyclic();
        void prepareRouting();
        void performRouting();
        void computeEdgeCount();
        void computeRowColumnSizes();
        void computeRowColumnPositions();
        void computeNodePositions();
        void precomputeEdgeCoordinates();

    private: // Algorithm functions
        LLEdge routeEdge(EdgesVector& m_horizedges, EdgesVector& m_vertedges, Matrix<bool>& m_edgevalid, LLBlock& start, LLBlock& end);
        int findHorizEdgeIndex(EdgesVector& edges, int row, int mincol, int maxcol) const;
        int findVertEdgeIndex(EdgesVector& edges, int col, int minrow, int maxrow) const;
        bool isEdgeMarked(EdgesVector& edges, int row, int col, int index) const;
        void markEdge(EdgesVector& edges, int row, int col, int index, bool used = true) const;
        void adjustGraphLayout(LLBlock & block, int col, int row);
        void computeLayout(LLBlock &block);

    private:
        template<typename T> static void removeFromDeque(std::deque<T>& v, T item) { v.erase(std::remove(v.begin(), v.end(), item), v.end()); }
        template<typename T> static void initDeque(std::deque<T>& v, size_t size, T value) { v.resize(size); for(size_t i = 0; i < size; i++) v[i] = value; }

    private:
        std::unordered_map<Node, LLBlock> m_blocks;
        std::deque<int> m_colx, m_rowy, m_coledgex, m_rowedgey, m_colwidth, m_rowheight, m_coledgecount, m_rowedgecount;
        std::deque<Node> m_blockorder;
        EdgesVector m_horizedges, m_vertedges;
        Matrix<bool> m_edgevalid;
        int m_layouttype;
};

} // namespace Graphing
} // namespace REDasm

#endif // LAYEREDLAYOUT_H
