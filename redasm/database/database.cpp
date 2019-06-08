#include "database.h"
#include <impl/database/database_impl.h>
#include "../disassembler/disassembler.h"
#include "../plugins/assembler/assembler.h"
#include "../plugins/loader/loader.h"
#include "../buffer/memorybuffer.h"
#include "../support/serializer.h"
#include "../support/utils.h"
#include "../context.h"
#include <fstream>

namespace REDasm {

bool Database::save(Disassembler *disassembler, const std::string &dbfilename, const std::string& filename)
{
    DatabaseImpl::m_lasterror.clear();
    std::fstream ofs(dbfilename, std::ios::out | std::ios::binary | std::ios::trunc);

    if(!ofs.is_open())
    {
        DatabaseImpl::m_lasterror = "Cannot save " + Utils::quoted(dbfilename);
        return false;
    }

    auto& document = disassembler->document();
    Loader* loader = disassembler->loader();
    Assembler* assembler = disassembler->assembler();
    ReferenceTable* references = disassembler->references();

    ofs.write(RDB_SIGNATURE, RDB_SIGNATURE_LENGTH);
    Serializer<u32>::write(ofs, RDB_VERSION);
    Serializer<u32>::write(ofs, static_cast<u32>(bits_count<size_t>::value)); // CLang 8.x workaround
    SerializerHelper::obfuscated(ofs, filename);
    Serializer<std::string>::write(ofs, loader->id());
    Serializer<std::string>::write(ofs, assembler->id());

    if(!SerializerHelper::compressed(ofs, loader->buffer()))
    {
        DatabaseImpl::m_lasterror = "Cannot compress database " + Utils::quoted(dbfilename);
        return false;
    }

    Serializer<ListingDocument>::write(ofs, document);
    Serializer<ReferenceTable>::write(ofs, references);
    return true;
}

Disassembler *Database::load(const std::string &dbfilename, std::string &filename)
{
    DatabaseImpl::m_lasterror.clear();
    std::fstream ifs(dbfilename, std::ios::in | std::ios::binary);

    if(!ifs.is_open())
    {
        DatabaseImpl::m_lasterror = "Cannot open " + Utils::quoted(dbfilename);
        return nullptr;
    }

    if(!SerializerHelper::signatureIs(ifs, RDB_SIGNATURE))
    {
        DatabaseImpl::m_lasterror = "Signature check failed for " + Utils::quoted(dbfilename);
        return nullptr;
    }

    u32 version = 0;
    Serializer<u32>::read(ifs, version);

    if(version != RDB_VERSION)
    {
        DatabaseImpl::m_lasterror = "Invalid version, got " + std::to_string(version) + " " + std::to_string(RDB_VERSION) + " required";
        return nullptr;
    }

    u32 rdbbits = 0;
    Serializer<u32>::read(ifs, rdbbits);

    if(bits_count<size_t>::value != rdbbits)
    {
        DatabaseImpl::m_lasterror = "Invalid bits: Expected " + std::to_string(bits_count<size_t>::value) + ", got " + std::to_string(rdbbits);
        return nullptr;
    }

    auto* buffer = new MemoryBuffer();
    std::string loaderid, assemblerid;
    SerializerHelper::deobfuscated(ifs, filename);
    Serializer<std::string>::read(ifs, loaderid);
    Serializer<std::string>::read(ifs, assemblerid);

    if(!SerializerHelper::decompressed(ifs, buffer))
    {
        DatabaseImpl::m_lasterror = "Cannot decompress database " + Utils::quoted(dbfilename);
        delete buffer;
        return nullptr;
    }

    const PluginInstance* loaderpi = r_pm->findLoader(loaderid);

    if(!loaderpi)
    {
        DatabaseImpl::m_lasterror = "Unsupported loader: " + Utils::quoted(loaderid);
        delete buffer;
        return nullptr;
    }

    const PluginInstance* assemblerpi = r_pm->findAssembler(assemblerid);

    if(!assemblerpi)
    {
        DatabaseImpl::m_lasterror = "Unsupported assembler: " + Utils::quoted(assemblerid);
        delete buffer;
        r_pm->unload(loaderpi);
        return nullptr;
     }

    LoadRequest request(filename, buffer);
    Loader* loader = static_cast<Loader*>(loaderpi->descriptor->plugin);
    loader->init(&request); // LoaderPlugin takes the ownership of the buffer

    auto* disassembler = new Disassembler(static_cast<Assembler*>(assemblerpi->descriptor->plugin), loader); // Take Ownership
    auto& document = disassembler->loader()->document();
    Serializer<ListingDocument>::read(ifs, document, std::bind(&Disassembler::disassembleInstruction, disassembler, std::placeholders::_1));
    ReferenceTable* references = disassembler->references();
    Serializer<ReferenceTable>::read(ifs, references);
    return disassembler;
}

const std::string &Database::lastError() { return DatabaseImpl::m_lasterror; }

} // namespace REDasm
