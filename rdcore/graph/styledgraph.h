#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include "datagraph.h"

typedef std::vector<RDGraphPoint> GraphPoints;

class StyledGraph: public DataGraph
{
    private:
        struct NodeAttributes { int x, y, width, height; };

        struct EdgeAttributes {
            std::string label, color;
            GraphPoints routes, arrow;
        };

    public:
        StyledGraph(Context* ctx);
        void clear() override;

    public: // Write
        void color(const RDGraphEdge* e, const std::string& s);
        void label(const RDGraphEdge* e, const std::string& s);
        void routes(const RDGraphEdge* e, const RDGraphPoint* p, size_t n);
        void arrow(const RDGraphEdge* e, const RDGraphPoint* p, size_t n);
        void areaWidth(int w);
        void areaHeight(int h);
        void x(RDGraphNode n, int px);
        void y(RDGraphNode n, int py);
        void width(RDGraphNode n, int w);
        void height(RDGraphNode n, int h);

    public: // Read
        const char* color(const RDGraphEdge* e) const;
        const char* label(const RDGraphEdge* e) const;
        size_t routes(const RDGraphEdge* e, const RDGraphPoint** path) const;
        size_t arrow(const RDGraphEdge* e, const RDGraphPoint** path) const;
        int areaWidth() const;
        int areaHeight() const;
        int x(RDGraphNode n) const;
        int y(RDGraphNode n) const;
        int width(RDGraphNode n) const;
        int height(RDGraphNode n) const;

    private:
        std::unordered_map<RDGraphNode, NodeAttributes> m_nodeattributes;
        std::unordered_map<RDGraphEdge, EdgeAttributes> m_edgeattributes;
        int m_areawidth{0}, m_areaheight{0};
};
