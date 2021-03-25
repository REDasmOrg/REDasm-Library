#include "loadertests.h"
#include "doctest.h"
#include "graphtests.h"
#include <algorithm>
#include <cstring>

void LoaderTests::testCavia(RDContext*, RDDocument* doc)
{
    LoaderTests::checkLabel(doc, 0x00401000, RD_ENTRY_NAME, AddressFlags_Function | AddressFlags_Exported);
    LoaderTests::checkLabel(doc, 0x00401029, "DlgProc_00401029", AddressFlags_Function);
    GraphTests::testCavia(doc);
}

void LoaderTests::testCM01(RDContext* ctx, RDDocument* doc)
{
    LoaderTests::checkLabel(doc, 0x00401128, "WndProc", AddressFlags_Function | AddressFlags_Exported);
    LoaderTests::checkLabel(doc, 0x00401253, "DlgProc_00401253", AddressFlags_Function);
    LoaderTests::checkLabel(doc, 0x0040130A, "DlgProc_0040130A", AddressFlags_Function);

    LoaderTests::checkLabel(doc, 0x004020E7, nullptr, AddressFlags_AsciiString);
    LoaderTests::checkLabel(doc, 0x00402129, nullptr, AddressFlags_AsciiString);
    LoaderTests::checkLabel(doc, 0x00402134, nullptr, AddressFlags_AsciiString);

    static std::map<rd_address, size_t> strings = {
        { 0x004020e7, 1 }, { 0x004020f4, 3 }, { 0x00402110, 1 },
        { 0x00402115, 1 }, { 0x0040211f, 1 }, { 0x00402129, 1 },
        { 0x00402134, 1 }, { 0x00402160, 2 }, { 0x00402169, 2 }
    };

    LoaderTests::checkLabelsAndRefs(ctx, doc, strings, AddressFlags_AsciiString);
    GraphTests::testCM01(doc);
}

void LoaderTests::testJmpTable(RDContext* ctx, RDDocument* doc)
{
    LoaderTests::checkLabel(doc, 0x00001362, "main", AddressFlags_Function);
    LoaderTests::checkLabel(doc, 0x00002570, "init", AddressFlags_Function);
    LoaderTests::checkLabel(doc, 0x000025e0, "fini", AddressFlags_Function);
    LoaderTests::checkLabel(doc, 0x00003007, nullptr, AddressFlags_AsciiString);
    LoaderTests::checkLabel(doc, 0x0000300c, nullptr, AddressFlags_AsciiString);
    LoaderTests::checkLabel(doc, 0x00003011, nullptr, AddressFlags_AsciiString);
    LoaderTests::checkLabel(doc, 0x00003016, nullptr, AddressFlags_AsciiString);
    LoaderTests::checkLabel(doc, 0x0000301b, nullptr, AddressFlags_AsciiString);
    LoaderTests::checkLabel(doc, 0x00003020, nullptr, AddressFlags_AsciiString);
    LoaderTests::checkLabel(doc, 0x00003025, nullptr, AddressFlags_AsciiString);
    LoaderTests::checkLabel(doc, 0x0000302a, nullptr, AddressFlags_AsciiString);
    LoaderTests::checkLabel(doc, 0x00003033, nullptr, AddressFlags_AsciiString);
    LoaderTests::checkLabel(doc, 0x00003044, nullptr, AddressFlags_AsciiString);
    LoaderTests::checkLabel(doc, 0x0000304f, nullptr, AddressFlags_AsciiString);
    LoaderTests::checkLabel(doc, 0x00003058, nullptr, AddressFlags_AsciiString);

    LoaderTests::checkLabel(doc, 0x00001219, nullptr, AddressFlags_Location);
    LoaderTests::checkLabel(doc, 0x00001248, nullptr, AddressFlags_Location);
    LoaderTests::checkLabel(doc, 0x00001277, nullptr, AddressFlags_Location);
    LoaderTests::checkLabel(doc, 0x000012a6, "sub_12A6", AddressFlags_Function);
    LoaderTests::checkLabel(doc, 0x000012d5, "sub_12D5", AddressFlags_Function);
    LoaderTests::checkLabel(doc, 0x00001304, "sub_1304", AddressFlags_Function);
    LoaderTests::checkLabel(doc, 0x00001333, "sub_1333", AddressFlags_Function);

    const RDNet* net = RDContext_GetNet(ctx);
    REQUIRE_EQ(RDNet_GetReferences(net, 0x00001219, nullptr), 1);
    REQUIRE_EQ(RDNet_GetReferences(net, 0x00001248, nullptr), 1);
    REQUIRE_EQ(RDNet_GetReferences(net, 0x00001277, nullptr), 1);
    REQUIRE_EQ(RDNet_GetReferences(net, 0x000012a6, nullptr), 0);
    REQUIRE_EQ(RDNet_GetReferences(net, 0x000012d5, nullptr), 0);
    REQUIRE_EQ(RDNet_GetReferences(net, 0x00001304, nullptr), 0);
    REQUIRE_EQ(RDNet_GetReferences(net, 0x00001333, nullptr), 0);

    GraphTests::testJmpTable(doc);
}

void LoaderTests::testSwitchCase(RDContext* ctx, RDDocument* doc)
{
    LoaderTests::checkLabel(doc, 0x00001169, "main", AddressFlags_Function);
    LoaderTests::checkLabel(doc, 0x00001270, "init", AddressFlags_Function);
    LoaderTests::checkLabel(doc, 0x000012e0, "fini", AddressFlags_Function);

    LoaderTests::checkLabel(doc, 0x00002004, nullptr, AddressFlags_AsciiString);
    LoaderTests::checkLabel(doc, 0x00002015, nullptr, AddressFlags_AsciiString);
    LoaderTests::checkLabel(doc, 0x00002018, nullptr, AddressFlags_AsciiString);
    LoaderTests::checkLabel(doc, 0x00002021, nullptr, AddressFlags_AsciiString);
    LoaderTests::checkLabel(doc, 0x0000202a, nullptr, AddressFlags_AsciiString);
    LoaderTests::checkLabel(doc, 0x00002035, nullptr, AddressFlags_AsciiString);
    LoaderTests::checkLabel(doc, 0x0000203f, nullptr, AddressFlags_AsciiString);
    LoaderTests::checkLabel(doc, 0x00002049, nullptr, AddressFlags_AsciiString);
    LoaderTests::checkLabel(doc, 0x00002052, nullptr, AddressFlags_AsciiString);
    LoaderTests::checkLabel(doc, 0x0000205d, nullptr, AddressFlags_AsciiString);

    const RDNet* net = RDContext_GetNet(ctx);
    REQUIRE_EQ(RDNet_GetReferences(net, 0x00002004, nullptr), 1);
    REQUIRE_EQ(RDNet_GetReferences(net, 0x00002015, nullptr), 1);
    REQUIRE_EQ(RDNet_GetReferences(net, 0x00002018, nullptr), 1);
    REQUIRE_EQ(RDNet_GetReferences(net, 0x00002021, nullptr), 1);
    REQUIRE_EQ(RDNet_GetReferences(net, 0x0000202a, nullptr), 1);
    REQUIRE_EQ(RDNet_GetReferences(net, 0x00002035, nullptr), 1);
    REQUIRE_EQ(RDNet_GetReferences(net, 0x0000203f, nullptr), 1);
    REQUIRE_EQ(RDNet_GetReferences(net, 0x00002049, nullptr), 1);
    REQUIRE_EQ(RDNet_GetReferences(net, 0x00002052, nullptr), 1);
    REQUIRE_EQ(RDNet_GetReferences(net, 0x0000205d, nullptr), 1);

    GraphTests::testSwitchCase(doc);
}

void LoaderTests::testVB5CrackMe(RDContext*, RDDocument* doc)
{
    static std::map<rd_address, const char*> vbevents;
    vbevents[0x004020C4] = "Form1_Command1_Click";

    LoaderTests::testVBEvents(doc, vbevents);
}

void LoaderTests::testTN_11(RDContext* ctx, RDDocument* doc)
{
    LoaderTests::checkLabel(doc, 0x004010c0, nullptr, AddressFlags_Function);
    LoaderTests::checkLabel(doc, 0x00405530, nullptr, AddressFlags_Pointer);
    LoaderTests::checkLabel(doc, 0x00405534, nullptr, AddressFlags_Pointer);
    LoaderTests::checkLabel(doc, 0x00405538, nullptr, AddressFlags_Pointer);
    LoaderTests::checkLabel(doc, 0x00405548, nullptr, AddressFlags_Location);

    LoaderTests::checkLabel(doc, 0x00401320, nullptr, AddressFlags_Pointer);
    LoaderTests::checkLabel(doc, 0x00401324, nullptr, AddressFlags_Pointer);
    LoaderTests::checkLabel(doc, 0x00401328, nullptr, AddressFlags_Pointer);
    LoaderTests::checkLabel(doc, 0x0040132c, nullptr, AddressFlags_Pointer);
    LoaderTests::checkLabel(doc, 0x00401330, nullptr, AddressFlags_Pointer);

    const RDNet* net = RDContext_GetNet(ctx);
    const RDReference* refs = nullptr;
    REQUIRE_EQ(RDNet_GetReferences(net, 0x00401320, &refs), 1);
    REQUIRE_EQ(refs[0].address, 0x401197);
    REQUIRE(HAS_FLAG(refs, ReferenceFlags_Direct));

    REQUIRE_EQ(RDNet_GetReferences(net, 0x00401324, nullptr), 0);
    REQUIRE_EQ(RDNet_GetReferences(net, 0x00401328, nullptr), 0);
    REQUIRE_EQ(RDNet_GetReferences(net, 0x0040132c, nullptr), 0);
    REQUIRE_EQ(RDNet_GetReferences(net, 0x00401330, nullptr), 0);
    REQUIRE_EQ(RDNet_GetReferences(net, 0x00401285, nullptr), 2);
    REQUIRE_EQ(RDNet_GetReferences(net, 0x004012fb, nullptr), 2);
    REQUIRE_EQ(RDNet_GetReferences(net, 0x0040119e, nullptr), 3);
    REQUIRE_EQ(RDNet_GetReferences(net, 0x00401238, nullptr), 2);
    REQUIRE_EQ(RDNet_GetReferences(net, 0x004010e8, nullptr), 3);
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
    LoaderTests::checkLabelsAndRefs(ctx, doc, strings, AddressFlags_WideString);
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
        REQUIRE_EQ(RDDocument_GetAddress(doc, name), address);

    LoaderTests::checkLabelsAndRefs(ctx, doc, strings, AddressFlags_AsciiString);
}

void LoaderTests::checkLabelsAndRefs(RDContext* ctx, RDDocument* doc, const std::map<rd_address, size_t>& labels, rd_flag flags)
{
    const RDNet* net = RDContext_GetNet(ctx);

    for(const auto& [address, refs] : labels)
    {
        LoaderTests::checkLabel(doc, address, nullptr, flags);
        if(refs != RD_NVAL) REQUIRE_EQ(RDNet_GetReferences(net, address, nullptr), refs);
    }
}

void LoaderTests::checkLabel(RDDocument* doc, rd_address address, const char* label, rd_flag flags)
{
    std::string casename = label ? label : ("Label @ " + rd_tohex(address));

    SUBCASE(casename.c_str())
    {
        const char* reslabel = RDDocument_GetLabel(doc, address);
        REQUIRE(reslabel);
        REQUIRE_EQ(RDDocument_GetAddress(doc, reslabel), address);

        if(flags != AddressFlags_None) REQUIRE((RDDocument_GetFlags(doc, address) & flags));
        if(label) REQUIRE(!std::strcmp(reslabel, label));
    }
}

void LoaderTests::checkLabel(RDDocument* doc, rd_address address, const char* label) { LoaderTests::checkLabel(doc, address, label, AddressFlags_None); }

void LoaderTests::testVBEvents(RDDocument* doc, const std::map<rd_address, const char*>& vbevents)
{
    for(const auto& [address, name] : vbevents)
        LoaderTests::checkLabel(doc, address, name, AddressFlags_Function);
}
