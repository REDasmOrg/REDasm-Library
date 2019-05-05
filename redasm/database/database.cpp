#include "database.h"
#include "../support/serializer.h"
#include "../plugins/assembler/assembler.h"
#include "../plugins/loader.h"
#include "../plugins/plugins.h"
#include <fstream>
#include <array>

namespace REDasm {

std::string Database::m_lasterror;

bool Database::save(DisassemblerAPI *disassembler, const std::string &dbfilename, const std::string& filename)
{
    m_lasterror.clear();
    std::fstream ofs(dbfilename, std::ios::out | std::ios::binary | std::ios::trunc);

    if(!ofs.is_open())
    {
        m_lasterror = "Cannot save " + REDasm::quoted(dbfilename);
        return false;
    }

    auto& document = disassembler->document();
    LoaderPlugin* loader = disassembler->loader();
    AssemblerPlugin* assembler = disassembler->assembler();
    ReferenceTable* references = disassembler->references();

    ofs.write(RDB_SIGNATURE, RDB_SIGNATURE_LENGTH);
    Serializer<u32>::write(ofs, RDB_VERSION);
    Serializer<u32>::write(ofs, bitwidth<size_t>::value);
    SerializerHelper::obfuscated(ofs, filename);
    Serializer<std::string>::write(ofs, loader->id());
    Serializer<std::string>::write(ofs, assembler->id());

    if(!SerializerHelper::compressed(ofs, loader->buffer()))
    {
        m_lasterror = "Cannot compress database " + REDasm::quoted(dbfilename);
        return false;
    }

    //document->serializeTo(ofs);
    Serializer<ReferenceTable>::write(ofs, references);
    return true;
}

Disassembler *Database::load(const std::string &dbfilename, std::string &filename)
{
    m_lasterror.clear();
    std::fstream ifs(dbfilename, std::ios::in | std::ios::binary);

    if(!ifs.is_open())
    {
        m_lasterror = "Cannot open " + REDasm::quoted(dbfilename);
        return nullptr;
    }

    if(!SerializerHelper::signatureIs(ifs, RDB_SIGNATURE))
    {
        m_lasterror = "Signature check failed for " + REDasm::quoted(dbfilename);
        return nullptr;
    }

    u32 version = 0;
    Serializer<u32>::read(ifs, version);

    if(version != RDB_VERSION)
    {
        m_lasterror = "Invalid version, got " + std::to_string(version) + " " + std::to_string(RDB_VERSION) + " required";
        return nullptr;
    }

    u32 rdbbits = 0;
    Serializer<u32>::read(ifs, rdbbits);

    if(bitwidth<size_t>::value != rdbbits)
    {
        m_lasterror = "Invalid bits: Expected " + std::to_string(bitwidth<size_t>::value) + ", got " + std::to_string(rdbbits);
        return nullptr;
    }

    auto* buffer = new MemoryBuffer();
    std::string loaderid, assemblerid;
    Serializer<std::string>::read(ifs, filename);
    Serializer<std::string>::read(ifs, loaderid);
    Serializer<std::string>::read(ifs, assemblerid);

    if(!SerializerHelper::decompressed(ifs, buffer))
    {
        m_lasterror = "Cannot decompress database " + REDasm::quoted(dbfilename);
        delete buffer;
        return nullptr;
    }

    const LoaderPlugin_Entry* loaderentry = REDasm::getLoader(loaderid);

    if(!loaderentry)
    {
        m_lasterror = "Unsupported loader: " + REDasm::quoted(loaderid);
        delete buffer;
        return nullptr;
    }

    LoadRequest request(filename, buffer);
    std::unique_ptr<LoaderPlugin> loader(loaderentry->init(request)); // LoaderPlugin takes the ownership of the buffer
    const AssemblerPlugin_Entry* assemblerentry = REDasm::getAssembler(assemblerid);

    if(!assemblerentry)
    {
        m_lasterror = "Unsupported assembler: " + REDasm::quoted(assemblerid);
        return nullptr;
    }

    //auto& document = loader->createDocument(); // Discard old document
    //document->deserializeFrom(ifs);

    auto* disassembler = new Disassembler(assemblerentry->init(), loader.release()); // Take ownership
    ReferenceTable* references = disassembler->references();
    Serializer<ReferenceTable>::read(ifs, references);
    return disassembler;
}

const std::string &Database::lastError() { return m_lasterror; }

} // namespace REDasm
