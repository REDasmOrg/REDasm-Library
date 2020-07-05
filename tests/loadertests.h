#pragma once

#include <map>
#include "../rdapi/rdapi.h"

class LoaderTests
{
    public:
        LoaderTests() = delete;
        static void testCavia(RDDisassembler*, RDDocument* doc);
        static void testCM01(RDDisassembler*, RDDocument* doc);
        static void testVB5CrackMe(RDDisassembler*, RDDocument* doc);
        static void testTN_11(RDDisassembler*d, RDDocument* doc);
        static void testSCrack(RDDisassembler*d, RDDocument* doc);

    private:
        static void checkSymbolsAndRefs(RDDisassembler* d, RDDocument* doc, const std::map<rd_address, size_t>& symbols, rd_type type, rd_flag flags);
        static void checkSymbol(RDDocument* doc, rd_address address, const char* name, rd_type type, rd_flag flags);
        static void testVBEvents(RDDocument* doc, const std::map<rd_address, const char*>& vbevents);
};

