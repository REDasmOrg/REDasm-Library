#pragma once

#include <rdapi/renderer/surface.h>
#include <vector>
#include <set>
#include "../object.h"

class Surface;

class SurfacePath : public Object
{
    public:
        SurfacePath(Surface* sf);
        size_t getPath(const RDPathItem** path) const;
        void update();

    private:
        void insertPath(rd_address fromaddress, rd_address toaddress);
        int calculateToRow(rd_address fromaddress, rd_address toaddress) const;

    private:
        std::set<std::pair<rd_address, rd_address>> m_done;
        std::vector<RDPathItem> m_paths;
        Surface* m_surface;
};

