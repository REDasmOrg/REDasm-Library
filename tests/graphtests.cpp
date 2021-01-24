#include "graphtests.h"
#include "doctest.h"

void GraphTests::testCavia(const RDDocument* doc)
{
    static TestGraphs g = {
        { 0x401000, 0xC9051DFC }, { 0x401029, 0x56D9A09B },
        { 0x401052, 0xC9051DFC }, { 0x401058, 0xC9051DFC },
        { 0x40105E, 0xC9051DFC }, { 0x401064, 0xC9051DFC },
    };

    GraphTests::testGraphs(g, doc);
}

void GraphTests::testCM01(const RDDocument* doc)
{
    static TestGraphs g = {
        { 0x401000, 0xA6C60AA5 }, { 0x401128, 0xC51CCFAF }, { 0x401253, 0xE77F25C6 },
        { 0x40130A, 0xA1A333A9 }, { 0x40134D, 0xC9051DFC }, { 0x401362, 0xC9051DFC },
        { 0x40137E, 0xF1CC36E9 }, { 0x4013C2, 0x9EA7F246 }, { 0x4013D2, 0xC9051DFC },
        { 0x4013D8, 0x1E0E029C }, { 0x40140A, 0xC9051DFC }, { 0x401416, 0xC9051DFC },
        { 0x401428, 0xC9051DFC }, { 0x401434, 0xC9051DFC }, { 0x40143A, 0xC9051DFC },
        { 0x401440, 0xC9051DFC }, { 0x40144C, 0xC9051DFC }, { 0x40145E, 0xC9051DFC },
        { 0x40146A, 0xC9051DFC }, { 0x401482, 0xC9051DFC }, { 0x401488, 0xC9051DFC },
        { 0x401494, 0xC9051DFC }, { 0x40149A, 0xC9051DFC }, { 0x4014A0, 0xC9051DFC },
        { 0x4014A6, 0xC9051DFC }, { 0x4014B2, 0xC9051DFC }, { 0x4014BE, 0xC9051DFC },
        { 0x4014D0, 0xC9051DFC }, { 0x4014D6, 0xC9051DFC }, { 0x401506, 0xC9051DFC },
        { 0x401512, 0xC9051DFC },
    };

    GraphTests::testGraphs(g, doc);
}

void GraphTests::testGraphs(const GraphTests::TestGraphs& tg, const RDDocument* doc)
{
    for(const auto& [address, hash] : tg)
    {
        RDGraph* g = nullptr;
        REQUIRE(RDDocument_GetFunctionGraph(doc,address, &g));
        REQUIRE_EQ(RDGraph_Hash(g), hash);
    }
}
