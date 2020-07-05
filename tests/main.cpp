#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <filesystem>
#include <functional>
#include <list>
#include "../rdapi/rdapi.h"
#include "loadertests.h"
#include "doctest.h"

static bool g_initialized = false;

namespace fs = std::filesystem;

typedef std::function<void(RDDisassembler*, RDDocument*)> TestCallback;
typedef std::pair<std::string, TestCallback> TestItem;

static std::list<TestItem> g_tests = {
    { "/home/davide/Programmazione/Cavia.exe", &LoaderTests::testCavia },
    { "/home/davide/Programmazione/Campioni/PE Test/CM01.exe", &LoaderTests::testCM01 },
    { "/home/davide/Programmazione/Campioni/PE Test/VB5CRKME.EXE", &LoaderTests::testVB5CrackMe },
    { "/home/davide/Programmazione/Campioni/PE Test/tn_11.exe", &LoaderTests::testTN_11 },
    { "/home/davide/Programmazione/Campioni/PE Test/tn12/scrack.exe", &LoaderTests::testSCrack }
};

void initializeContext()
{
    if(g_initialized) return;

    RD_AddPluginPath((fs::current_path().parent_path().parent_path() / "plugins").c_str());
    RD_SetStatusCallback([](const char*, void*) { }, nullptr);
    RD_SetLogCallback([](const char*, void*) { }, nullptr);
    RD_InitContext();
    g_initialized = true;
}

static RDLoaderPlugin* g_currloader = nullptr;

TEST_CASE("Executables")
{
    initializeContext();

    for(const auto& [filepath, test] : g_tests)
    {
        SUBCASE(fs::path(filepath).filename().c_str())
        {
            RDBuffer* b = RDBuffer_CreateFromFile(filepath.c_str());
            RDLoaderRequest req = { filepath.c_str(), b };

            RD_GetLoaders(&req, [](RDLoaderPlugin* p, void* userdata) {
                auto** loader = reinterpret_cast<RDLoaderPlugin**>(userdata);

                if(*loader) RDPlugin_Free(reinterpret_cast<RDPluginHeader*>(p));
                else *loader = p;

            }, &g_currloader);

            REQUIRE(g_currloader);

            RDAssemblerPlugin* assembler = RDLoader_GetAssembler(g_currloader);
            REQUIRE(assembler);

            RDDisassembler* disassembler = RDDisassembler_Create(&req, g_currloader, assembler);
            RDLoader* loader = RDDisassembler_GetLoader(disassembler);
            REQUIRE(loader);
            REQUIRE(RDLoader_Load(loader));
            RD_Disassemble(disassembler);
            test(disassembler, RDDisassembler_GetDocument(disassembler));
            RD_Free(disassembler);
            g_currloader = nullptr;
        }
    }
}
