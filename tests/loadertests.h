#pragma once

#include <map>
#include "../rdapi/rdapi.h"

class LoaderTests
{
    public:
        LoaderTests() = delete;
        static void testCavia(RDContext*, RDDocument* doc);
        static void testCM01(RDContext*, RDDocument* doc);
        static void testVB5CrackMe(RDContext*, RDDocument* doc);
        static void testTN_11(RDContext*d, RDDocument* doc);
        static void testSCrack(RDContext*d, RDDocument* doc);

    private:
        static void checkSymbolsAndRefs(RDContext* ctx, RDDocument* doc, const std::map<rd_address, size_t>& symbols, rd_type type, rd_flag flags);
        static void checkSymbol(RDDocument* doc, rd_address address, const char* name, rd_type type, rd_flag flags);
        static void testVBEvents(RDDocument* doc, const std::map<rd_address, const char*>& vbevents);
};

