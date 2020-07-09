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
        { 0x401000, 0xD7CF8DF5 }, { 0x401128, 0xA14B397F }, { 0x401253, 0x04B375BF },
        { 0x40130A, 0x96351D03 }, { 0x40134D, 0xC9051DFC }, { 0x401362, 0xC9051DFC },
        { 0x40137E, 0xFDB137AF }, { 0x4013C2, 0xAA0AA611 }, { 0x4013D2, 0xC9051DFC },
        { 0x4013D8, 0x149DF600 }, { 0x40140A, 0xC9051DFC }, { 0x401416, 0xC9051DFC },
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
    size_t c = RDDocument_FunctionsCount(doc);

    for(size_t i = 0; i < c; i++)
    {
        auto loc = RDDocument_GetFunctionAt(doc, i);
        REQUIRE(loc.valid);

        RDGraph* g = nullptr;
        REQUIRE(RDDocument_GetFunctionGraph(doc, loc.address, &g));

        auto it = tg.find(RDFunctionGraph_GetStartAddress(g));
        REQUIRE(it != tg.end());
        REQUIRE_EQ(RDGraph_Hash(g), it->second);
    }
}
