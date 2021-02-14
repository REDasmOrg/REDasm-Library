#pragma once

#include "../rdapi/rdapi.h"
#include <map>

class GraphTests
{
    private:
        typedef std::map<rd_address, u32> TestGraphs;

    public:
        GraphTests() = delete;
        static void testCavia(const RDDocument* doc);
        static void testCM01(const RDDocument* doc);
        static void testJmpTable(const RDDocument* doc);
        static void testSwitchCase(const RDDocument* doc);

    private:
        static void testGraphs(const TestGraphs& tg, const RDDocument* doc);
};

