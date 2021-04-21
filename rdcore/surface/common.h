#pragma once

#include <rdapi/renderer/surface.h>
#include <string>
#include <deque>
#include <vector>

struct SurfaceChunk
{
    u8 background, foreground;
    std::string chunk;
};

struct SurfaceRow
{
    SurfaceRow() = default;
    SurfaceRow(rd_address address): address(address) { }
    SurfaceRow(rd_address address, bool isvirtual): isvirtual(isvirtual), address(address) { }

    bool isvirtual{false};
    rd_address address{RD_NVAL};
    std::string text;
    std::deque<SurfaceChunk> chunks;
    std::vector<RDSurfaceCell> cells;
};
