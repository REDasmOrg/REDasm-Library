#include "loadertests.h"
#include "doctest.h"
#include "graphtests.h"
#include <algorithm>
#include <cstring>

void LoaderTests::testCavia(RDContext*, RDDocument* doc)
{
    LoaderTests::checkSymbol(doc, 0x00401000, RD_ENTRY_NAME, SymbolType_Function, SymbolFlags_EntryPoint);
    LoaderTests::checkSymbol(doc, 0x00401029, "DlgProc_00401029", SymbolType_Function, SymbolFlags_None);
    GraphTests::testCavia(doc);
}

void LoaderTests::testCM01(RDContext* ctx, RDDocument* doc)
{
    LoaderTests::checkSymbol(doc, 0x00401128, "WndProc", SymbolType_Function, SymbolFlags_Export);
    LoaderTests::checkSymbol(doc, 0x00401253, "DlgProc_00401253", SymbolType_Function, SymbolFlags_None);
    LoaderTests::checkSymbol(doc, 0x0040130A, "DlgProc_0040130A", SymbolType_Function, SymbolFlags_None);

    LoaderTests::checkSymbol(doc, 0x004020E7, nullptr, SymbolType_String, SymbolFlags_AsciiString);
    LoaderTests::checkSymbol(doc, 0x00402129, nullptr, SymbolType_String, SymbolFlags_AsciiString);
    LoaderTests::checkSymbol(doc, 0x00402134, nullptr, SymbolType_String, SymbolFlags_AsciiString);

    static std::map<rd_address, size_t> strings = {
        { 0x004020e7, 1 }, { 0x004020f4, 3 }, { 0x00402110, 1 },
        { 0x00402115, 1 }, { 0x0040211f, 1 }, { 0x00402129, 1 },
        { 0x00402134, 1 }, { 0x00402160, 2 }, { 0x00402169, 2 }
    };

    LoaderTests::checkSymbolsAndRefs(ctx, doc, strings, SymbolType_String, SymbolFlags_AsciiString);
    GraphTests::testCM01(doc);
}

void LoaderTests::testVB5CrackMe(RDContext*, RDDocument* doc)
{
    static std::map<rd_address, const char*> vbevents;
    vbevents[0x004020C4] = "Form1_Command1_Click";

    LoaderTests::testVBEvents(doc, vbevents);
}

void LoaderTests::testTN_11(RDContext* ctx, RDDocument* doc)
{
    LoaderTests::checkSymbol(doc, 0x004010c0, nullptr, SymbolType_Function, SymbolFlags_None);
    LoaderTests::checkSymbol(doc, 0x00405530, nullptr, SymbolType_Data, SymbolFlags_Pointer);
    LoaderTests::checkSymbol(doc, 0x00405534, nullptr, SymbolType_Data, SymbolFlags_Pointer);
    LoaderTests::checkSymbol(doc, 0x00405538, nullptr, SymbolType_Data, SymbolFlags_Pointer);
    LoaderTests::checkSymbol(doc, 0x00405548, nullptr, SymbolType_Data, SymbolFlags_None);

    const RDNet* net = RDContext_GetNet(ctx);
    REQUIRE_EQ(RDNet_GetReferences(net, 0x00405548, nullptr), 2);
}

void LoaderTests::testSCrack(RDContext* ctx, RDDocument* doc)
{
    static std::map<rd_address, const char*> vbevents;
    vbevents[0x00403BB0] = "main_about_Click";
    vbevents[0x00403D20] = "main_about_GotFocus";
    vbevents[0x00403DE0] = "main_about_LostFocus";
    vbevents[0x00403EA0] = "main_register_Click";
    vbevents[0x00404970] = "main_register_GotFocus";
    vbevents[0x00404A30] = "main_register_LostFocus";
    vbevents[0x00404AF0] = "main_sn_GotFocus";
    vbevents[0x00404BB0] = "main_sn_LostFocus";
    vbevents[0x00404C70] = "main_uname_GotFocus";
    vbevents[0x00404D30] = "main_uname_LostFocus";
    vbevents[0x00404DF0] = "aboutfrm_ok_Click";
    vbevents[0x00404EE0] = "aboutfrm_pmode_Click";
    vbevents[0x00404FE0] = "aboutfrm_uic_Click";

    static std::map<rd_address, size_t> strings = {
        { 0x00402ad8, 1 }, { 0x00402b1c, 2 }, { 0x00402b2c, 1 },
        { 0x00402b78, 1 }, { 0x00402d48, 2 }, { 0x00402d58, 1 },
        { 0x00402de8, 1 }
    };

    LoaderTests::testVBEvents(doc, vbevents);
    LoaderTests::checkSymbolsAndRefs(ctx, doc, strings, SymbolType_String, SymbolFlags_WideString);
}

void LoaderTests::testIOLI00(RDContext* ctx, RDDocument* doc)
{
    static std::map<rd_address, const char*> functions = {
        { 0x00011000, "WinMainCRTStartup" },
        { 0x00011050, "main" },
        { 0x000116C8, "GetModuleFileNameW"}
    };

    static std::map<rd_address, size_t> strings = {
        { 0x00013000, 2 }, { 0x00013018, 2 }, { 0x00013024, 2 },
        { 0x00013028, 2 }, { 0x00013030, 2 }, { 0x00013044, 2 },
    };

    for(const auto& [address, name] : functions)
    {
        RDSymbol symbol;
        REQUIRE(RDDocument_GetSymbolByName(doc, name, &symbol));
        REQUIRE_EQ(symbol.address, address);
    }

    LoaderTests::checkSymbolsAndRefs(ctx, doc, strings, SymbolType_String, SymbolFlags_AsciiString);
}

void LoaderTests::checkSymbolsAndRefs(RDContext* ctx, RDDocument* doc, const std::map<rd_address, size_t>& symbols, rd_type type, rd_flag flags)
{
    const RDNet* net = RDContext_GetNet(ctx);

    for(const auto& [address, refs] : symbols)
    {
        LoaderTests::checkSymbol(doc, address, nullptr, type, flags);
        if(refs != RD_NVAL) REQUIRE_EQ(RDNet_GetReferences(net, address, nullptr), refs);
    }
}

void LoaderTests::checkSymbol(RDDocument* doc, rd_address address, const char* name, rd_type type, rd_flag flags)
{
    std::string casename = name ? name : ("Symbol @ " + rd_tohex(address));

    SUBCASE(casename.c_str())
    {
        RDSymbol symbol;
        REQUIRE(RDDocument_GetSymbolByAddress(doc, address, &symbol));
        REQUIRE_EQ(symbol.address, address);

        if(type != SymbolType_None) REQUIRE_EQ(symbol.type, type);
        if(flags != SymbolFlags_None) REQUIRE((symbol.flags & flags));
        if(name) REQUIRE(!std::strcmp(RDDocument_GetSymbolName(doc, address), name));
    }
}

void LoaderTests::testVBEvents(RDDocument* doc, const std::map<rd_address, const char*>& vbevents)
{
    for(const auto& [address, name] : vbevents)
        LoaderTests::checkSymbol(doc, address, name, SymbolType_Function, SymbolFlags_None);
}
