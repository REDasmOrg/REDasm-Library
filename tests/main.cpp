#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <filesystem>
#include <functional>
#include <list>
#include "../rdapi/rdapi.h"
#include "loadertests.h"
#include "doctest.h"

static bool g_initialized = false;

namespace fs = std::filesystem;

typedef std::function<void(RDContext*, RDDocument*)> TestCallback;
typedef std::pair<std::string, TestCallback> TestItem;

static std::list<TestItem> g_tests = {
    { "/home/davide/Programmazione/Cavia.exe", &LoaderTests::testCavia },
    { "/home/davide/Programmazione/Campioni/PE Test/CM01.exe", &LoaderTests::testCM01 },
    { "/home/davide/Programmazione/Campioni/ELF Test/jmptable", &LoaderTests::testJmpTable },
    { "/home/davide/Programmazione/Campioni/ELF Test/switch_case", &LoaderTests::testSwitchCase },
    { "/home/davide/Programmazione/Campioni/PE Test/VB5CRKME.EXE", &LoaderTests::testVB5CrackMe },
    { "/home/davide/Programmazione/Campioni/PE Test/tn_11.exe", &LoaderTests::testTN_11 },
    { "/home/davide/Programmazione/Campioni/PE Test/tn12/scrack.exe", &LoaderTests::testSCrack },
    { "/home/davide/Programmazione/Campioni/IOLI-crackme/bin-pocketPC/crackme0x00.arm.exe", &LoaderTests::testIOLI00 },
    { "/home/davide/Programmazione/Campioni/ELF Test/seCuRe_Cmd.arm", &LoaderTests::testSecureCMD }
};

void initializeContext()
{
    if(g_initialized) return;

    RDConfig_SetLogLevel(LogLevel_Off);
    RDConfig_AddPluginPath((fs::current_path().parent_path().parent_path() / "plugins").string().c_str());
    RDConfig_SetStatusCallback([](const char*, void*) { }, nullptr);
    RDConfig_SetLogCallback([](const char*, void*) { }, nullptr);
    g_initialized = true;
}

static const RDEntryLoader* g_currloader = nullptr;

TEST_CASE("Executables")
{
    initializeContext();

    for(const auto& [filepath, test] : g_tests)
    {
        SUBCASE(fs::path(filepath).filename().string().c_str())
        {
            rd_ptr<RDContext> ctx(RDContext_Create());
            RDBuffer* b = RDBuffer_CreateFromFile(filepath.c_str());
            RDLoaderRequest req = { filepath.c_str(), b, { } };

            RDContext_FindLoaderEntries(ctx.get(), &req, [](const RDEntryLoader* entry, void* userdata) {
                const auto** currloader = reinterpret_cast<const RDEntryLoader**>(userdata);
                if(*currloader) return;
                *currloader = entry;
            }, &g_currloader);

            REQUIRE(g_currloader);
            REQUIRE(RDContext_Bind(ctx.get(), &req, g_currloader, nullptr));

            RDContext_Disassemble(ctx.get());
            test(ctx.get(), RDContext_GetDocument(ctx.get()));
            g_currloader = nullptr;
        }
    }
}
