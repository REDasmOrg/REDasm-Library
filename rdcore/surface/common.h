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

    rd_address address;
    std::string text;
    std::deque<SurfaceChunk> chunks;
    std::vector<RDSurfaceCell> cells;
};
