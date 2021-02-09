#include "graphtests.h"
#include "doctest.h"

void GraphTests::testCavia(const RDDocument* doc)
{
    static TestGraphs g = {
        { 0x401000, 0xC9051DFC }, { 0x401029, 0x6711D964 },
        { 0x401052, 0xC9051DFC }, { 0x401058, 0xC9051DFC },
        { 0x40105E, 0xC9051DFC }, { 0x401064, 0xC9051DFC },
    };

    GraphTests::testGraphs(g, doc);
}

void GraphTests::testCM01(const RDDocument* doc)
{
    static TestGraphs g = {
        { 0x00401000, 0x29521D06 }, { 0x00401128, 0x4070F071 }, { 0x00401253, 0x60F31F9D },
        { 0x0040130A, 0x0F710C9D }, { 0x0040134D, 0xC9051DFC }, { 0x00401362, 0xC9051DFC },
        { 0x0040137E, 0x98916B68 }, { 0x004013C2, 0x49904259 }, { 0x004013D2, 0xC9051DFC },
        { 0x004013D8, 0x09E26C53 }, { 0x0040140A, 0xC9051DFC }, { 0x00401416, 0xC9051DFC },
        { 0x00401428, 0xC9051DFC }, { 0x00401434, 0xC9051DFC }, { 0x0040143A, 0xC9051DFC },
        { 0x00401440, 0xC9051DFC }, { 0x0040144C, 0xC9051DFC }, { 0x0040145E, 0xC9051DFC },
        { 0x0040146A, 0xC9051DFC }, { 0x00401482, 0xC9051DFC }, { 0x00401488, 0xC9051DFC },
        { 0x00401494, 0xC9051DFC }, { 0x0040149A, 0xC9051DFC }, { 0x004014A0, 0xC9051DFC },
        { 0x004014A6, 0xC9051DFC }, { 0x004014B2, 0xC9051DFC }, { 0x004014BE, 0xC9051DFC },
        { 0x004014D0, 0xC9051DFC }, { 0x004014D6, 0xC9051DFC }, { 0x00401506, 0xC9051DFC },
        { 0x00401512, 0xC9051DFC },
    };

    GraphTests::testGraphs(g, doc);
}

void GraphTests::testGraphs(const GraphTests::TestGraphs& tg, const RDDocument* doc)
{
    for(const auto& [address, hash] : tg)
    {
        RDGraph* g = nullptr;
        REQUIRE(RDDocument_GetFunctionGraph(doc, address, &g));
        REQUIRE_EQ(RDGraph_Hash(g), hash);
    }
}
