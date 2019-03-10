#define WRAP_TO_STRING(...)         #__VA_ARGS__
#define FORMAT_PLUGIN(format)       WRAP_TO_STRING(../formats/format/format.h)
#define ASSEMBLER_PLUGIN(assembler) WRAP_TO_STRING(../assemblers/assembler/assembler.h)

#include <algorithm>
#include "plugins.h"

/* *** Formats *** */
#include FORMAT_PLUGIN(binary)
#include FORMAT_PLUGIN(chip8)
#include FORMAT_PLUGIN(pe)
#include FORMAT_PLUGIN(elf)
#include FORMAT_PLUGIN(psxexe)
#include FORMAT_PLUGIN(dex)
#include FORMAT_PLUGIN(xbe)
#include FORMAT_PLUGIN(gba)
#include FORMAT_PLUGIN(n64)

/* *** Assemblers *** */
#include ASSEMBLER_PLUGIN(x86)
#include ASSEMBLER_PLUGIN(mips)
#include ASSEMBLER_PLUGIN(dalvik)
#include ASSEMBLER_PLUGIN(cil)
#include ASSEMBLER_PLUGIN(metaarm)
//#include ASSEMBLER_PLUGIN(arm64)
#include ASSEMBLER_PLUGIN(chip8)

#define EXT_LIST(...) { __VA_ARGS__ }
#define REGISTER_FORMAT_PLUGIN_EXT(ext, desc, id) registerFormatByExt(#ext, desc, &id##_formatPlugin); Plugins::formatsCount++

#define REGISTER_FORMAT_PLUGIN_EXT_LIST(extlist, desc, id) for(const auto& ext : extlist) \
                                                             registerFormatByExt(ext, desc, &id##_formatPlugin); \
                                                           Plugins::formatsCount++;

#define REGISTER_FORMAT_PLUGIN(id)                REDasm::Plugins::formats.emplace_front(&id##_formatPlugin); Plugins::formatsCount++
#define REGISTER_ASSEMBLER_PLUGIN(id)             REDasm::Plugins::assemblers[#id] = &id##_assemblerPlugin

namespace REDasm {

size_t Plugins::formatsCount = 0;
EntryListT<FormatPlugin_Entry>::Type Plugins::formats;
EntryMapT<FormatEntryListByExt>::Type Plugins::formatsByExt;
EntryMapT<AssemblerPlugin_Entry>::Type Plugins::assemblers;

static void registerFormatByExt(std::string ext, const std::string& description, const FormatPlugin_Entry& cb)
{
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    auto it = Plugins::formatsByExt.find(ext);

    if(it != Plugins::formatsByExt.end())
        it->second.emplace_front(std::make_pair(cb, description));
    else
        Plugins::formatsByExt[ext] = { std::make_pair(cb, description) };
}

void init(const std::string& temppath, const std::string& searchpath)
{
    Runtime::rntTempPath = temppath;
    Runtime::rntSearchPath = searchpath;

    // By format
    REGISTER_FORMAT_PLUGIN(binary); // Always last choice
    REGISTER_FORMAT_PLUGIN(n64rom);
    REGISTER_FORMAT_PLUGIN(gbarom);
    REGISTER_FORMAT_PLUGIN(xbe);
    REGISTER_FORMAT_PLUGIN(dex);
    REGISTER_FORMAT_PLUGIN(psxexe);
    REGISTER_FORMAT_PLUGIN(elf64be);
    REGISTER_FORMAT_PLUGIN(elf64le);
    REGISTER_FORMAT_PLUGIN(elf32be);
    REGISTER_FORMAT_PLUGIN(elf32le);
    REGISTER_FORMAT_PLUGIN(pe64);
    REGISTER_FORMAT_PLUGIN(pe32);

    // By extension
    REGISTER_FORMAT_PLUGIN_EXT_LIST(EXT_LIST("chip8", "ch8", "rom"), "CHIP-8 Rom", chip8);

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

FormatPlugin *getFormat(AbstractBuffer *buffer)
{
    for(const FormatPlugin_Entry& formatentry : Plugins::formats)
    {
        FormatPlugin* fp = formatentry(buffer);

        if(fp)
            return fp;
    }

    return nullptr;
}

AssemblerPlugin *getAssembler(const std::string& id)
{
    auto it = REDasm::findPluginEntry<AssemblerPlugin_Entry>(id, Plugins::assemblers);

    if(it != Plugins::assemblers.end())
        return it->second();

    return nullptr;
}

void setLoggerCallback(const Runtime::LogCallback& logcb) { Runtime::rntLogCallback = logcb; }
void setStatusCallback(const Runtime::LogCallback& logcb) { Runtime::rntStatusCallback = logcb; }
void setProgressCallback(const Runtime::ProgressCallback& pcb) { Runtime::rntProgressCallback = pcb; }

bool getFormatsByExt(std::string ext, FormatEntryListByExt **entries)
{
    auto it = Plugins::formatsByExt.find(ext);

    if(it == Plugins::formatsByExt.end())
        return false;

    *entries = &it->second;
    return true;
}

}
