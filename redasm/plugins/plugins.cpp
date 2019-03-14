#define WRAP_TO_STRING(...)         #__VA_ARGS__
#define LOADER_PLUGIN(loadername)   WRAP_TO_STRING(../loaders/loadername/loadername.h)
#define ASSEMBLER_PLUGIN(assembler) WRAP_TO_STRING(../assemblers/assembler/assembler.h)

#include <algorithm>
#include "plugins.h"

/* *** Loaders *** */
#include LOADER_PLUGIN(binary)
#include LOADER_PLUGIN(chip8)
#include LOADER_PLUGIN(pe)
#include LOADER_PLUGIN(elf)
#include LOADER_PLUGIN(psxexe)
#include LOADER_PLUGIN(dex)
#include LOADER_PLUGIN(xbe)
#include LOADER_PLUGIN(gba)
#include LOADER_PLUGIN(n64)

/* *** Assemblers *** */
#include ASSEMBLER_PLUGIN(x86)
#include ASSEMBLER_PLUGIN(mips)
#include ASSEMBLER_PLUGIN(dalvik)
#include ASSEMBLER_PLUGIN(cil)
#include ASSEMBLER_PLUGIN(metaarm)
//#include ASSEMBLER_PLUGIN(arm64)
#include ASSEMBLER_PLUGIN(chip8)

#define REGISTER_LOADER_PLUGIN(id)                REDasm::Plugins::loaders.push_front(LOADER_PLUGIN_ENTRY(id)); Plugins::loadersCount++
#define REGISTER_ASSEMBLER_PLUGIN(id)             REDasm::Plugins::assemblers[#id] = ASSEMBLER_PLUGIN_ENTRY(id)

namespace REDasm {

size_t Plugins::loadersCount = 0;
EntryListT<LoaderPlugin_Entry>::Type Plugins::loaders;
EntryMapT<AssemblerPlugin_Entry>::Type Plugins::assemblers;

void init(const std::string& temppath, const std::string& searchpath)
{
    Runtime::rntTempPath = temppath;
    Runtime::rntSearchPath = searchpath;

    REGISTER_LOADER_PLUGIN(binary); // Always last choice
    REGISTER_LOADER_PLUGIN(chip8);
    REGISTER_LOADER_PLUGIN(n64rom);
    REGISTER_LOADER_PLUGIN(gbarom);
    REGISTER_LOADER_PLUGIN(xbe);
    REGISTER_LOADER_PLUGIN(dex);
    REGISTER_LOADER_PLUGIN(psxexe);
    REGISTER_LOADER_PLUGIN(elf64be);
    REGISTER_LOADER_PLUGIN(elf64le);
    REGISTER_LOADER_PLUGIN(elf32be);
    REGISTER_LOADER_PLUGIN(elf32le);
    REGISTER_LOADER_PLUGIN(pe64);
    REGISTER_LOADER_PLUGIN(pe32);

    // Assemblers
    REGISTER_ASSEMBLER_PLUGIN(x86_16);
    REGISTER_ASSEMBLER_PLUGIN(x86_32);
    REGISTER_ASSEMBLER_PLUGIN(x86_64);
    REGISTER_ASSEMBLER_PLUGIN(mips32le);
    REGISTER_ASSEMBLER_PLUGIN(mips64le);
    REGISTER_ASSEMBLER_PLUGIN(mips32r6le);
    REGISTER_ASSEMBLER_PLUGIN(mips2le);
    REGISTER_ASSEMBLER_PLUGIN(mips3le);
    REGISTER_ASSEMBLER_PLUGIN(mipsmicrole);
    REGISTER_ASSEMBLER_PLUGIN(mips32be);
    REGISTER_ASSEMBLER_PLUGIN(mips64be);
    REGISTER_ASSEMBLER_PLUGIN(mips32r6be);
    REGISTER_ASSEMBLER_PLUGIN(mips2be);
    REGISTER_ASSEMBLER_PLUGIN(mips3be);
    REGISTER_ASSEMBLER_PLUGIN(mipsmicrobe);
    REGISTER_ASSEMBLER_PLUGIN(arm);
    REGISTER_ASSEMBLER_PLUGIN(armthumb);
    REGISTER_ASSEMBLER_PLUGIN(metaarm);
    //REGISTER_ASSEMBLER_PLUGIN(arm64);
    REGISTER_ASSEMBLER_PLUGIN(dalvik);
    REGISTER_ASSEMBLER_PLUGIN(cil);
    REGISTER_ASSEMBLER_PLUGIN(chip8);
}

const AssemblerPlugin_Entry *getAssembler(const std::string& id)
{
    auto it = REDasm::findPluginEntry<AssemblerPlugin_Entry>(id, Plugins::assemblers);

    if(it != Plugins::assemblers.end())
        return &it->second;

    return nullptr;
}

void setLoggerCallback(const Runtime::LogCallback& logcb) { Runtime::rntLogCallback = logcb; }
void setStatusCallback(const Runtime::LogCallback& logcb) { Runtime::rntStatusCallback = logcb; }
void setProgressCallback(const Runtime::ProgressCallback& pcb) { Runtime::rntProgressCallback = pcb; }

LoaderList getLoaders(const LoadRequest &request, bool skipbinaries)
{
    LoaderList loaders;

    for(const LoaderPlugin_Entry& entry : Plugins::loaders)
    {
        if(!entry.test(request) || (skipbinaries && (entry.flags() & LoaderFlags::Binary)))
            continue;

        loaders.push_back(&entry);
    }

    return loaders;
}

const LoaderPlugin_Entry *getLoader(const std::string &id)
{
    for(const LoaderPlugin_Entry& entry : Plugins::loaders)
    {
        if(entry.id() == id)
            return &entry;
    }

    return nullptr;
}

}
