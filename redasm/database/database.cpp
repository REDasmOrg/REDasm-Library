#include "database.h"
#include <impl/database/database_impl.h>
#include <impl/libs/cereal/archives/binary.hpp>
#include "../disassembler/disassembler.h"
#include "../plugins/assembler/assembler.h"
#include "../plugins/loader/loader.h"
#include "../buffer/memorybuffer.h"
#include "../support/compression.h"
#include "../support/utils.h"
#include "../context.h"
#include <fstream>

namespace REDasm {

bool Database::save(Disassembler *disassembler, const String &dbfilename, const String& filename)
{
    DatabaseImpl::m_lasterror.clear();
    std::fstream ofs(dbfilename.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);

    if(!ofs.is_open())
    {
        DatabaseImpl::m_lasterror = "Cannot save " + dbfilename.quoted();
        return false;
    }

    // Write Header
    ofs.write(reinterpret_cast<const char*>(&DatabaseImpl::m_header), sizeof(RDBHeader));

    // Write Content
    const auto* loader = disassembler->loader();
    const auto* assembler = disassembler->assembler();

    cereal::BinaryOutputArchive out(ofs);

    // ... FileName, LoaderID, AssemblerID
    out(filename.xorified(), loader->id(), assembler->id());

    // ... Buffer
    MemoryBuffer mb;

    if(!Compression::deflate(loader->buffer(), &mb))
    {
        DatabaseImpl::m_lasterror = "Cannot decompress database " + dbfilename.quoted();
        return false;
    }

    mb.save(out);

    // ... Document
    auto lock = s_lock_safe_ptr(loader->document());
    lock->save(out);

    // ... References
    disassembler->references()->save(out);
    return true;
}

Disassembler *Database::load(const String &dbfilename, String &filename)
{
    DatabaseImpl::m_lasterror.clear();
    std::fstream ifs(dbfilename.c_str(), std::ios::in | std::ios::binary);

    if(!ifs.is_open())
    {
        DatabaseImpl::m_lasterror = "Cannot open " + dbfilename.quoted();
        return nullptr;
    }

    // Read Header
    RDBHeader rdbheader;
    ifs.read(reinterpret_cast<char*>(&rdbheader), sizeof(RDBHeader));

    if(std::strncmp(rdbheader.signature, DatabaseImpl::m_header.signature, RDB_SIGNATURE_LENGTH))
    {
        DatabaseImpl::m_lasterror = "Signature check failed for " + dbfilename.quoted();
        return nullptr;
    }

    if(rdbheader.version != RDB_VERSION)
    {
        DatabaseImpl::m_lasterror = "Invalid version, got " + String::number(rdbheader.version) + " " + String::number(RDB_VERSION) + " required";
        return nullptr;
    }

    if(rdbheader.bits != bits_count<size_t>::value)
    {
        DatabaseImpl::m_lasterror = "Invalid bits: Expected " + String::number(bits_count<size_t>::value) + ", got " + String::number(rdbheader.bits);
        return nullptr;
    }

    // Read Content
    cereal::BinaryInputArchive in(ifs);

    // ... FileName, LoaderID, AssemblerID
    String loaderid, assemblerid;
    in(filename, loaderid, assemblerid);
    filename = filename.xorified();

    // ... Buffer
    MemoryBuffer mb, *buffer = new MemoryBuffer();
    mb.load(in);

    if(!Compression::inflate(&mb, buffer))
    {
        DatabaseImpl::m_lasterror = "Cannot decompress database " + dbfilename.quoted();
        delete buffer;
        return nullptr;
    }

    const PluginInstance* loaderpi = r_pm->findLoader(loaderid);

    if(!loaderpi)
    {
        DatabaseImpl::m_lasterror = "Unsupported loader: " + loaderid.quoted();
        delete buffer;
        return nullptr;
    }

    const PluginInstance* assemblerpi = r_pm->findAssembler(assemblerid.c_str());

    if(!assemblerpi)
    {
        DatabaseImpl::m_lasterror = "Unsupported assembler: " + assemblerid.quoted();
        delete buffer;
        r_pm->unload(loaderpi);
        return nullptr;
     }

    LoadRequest request(filename, buffer);
    Loader* loader = static_cast<Loader*>(loaderpi->descriptor->plugin);
    loader->init(request);

    // Take Assembler/Loader/Buffer Ownership, bind to Context
    //auto* disassembler = new Disassembler(static_cast<Assembler*>(assemblerpi->descriptor->plugin), loader);
    //disassembler->document()->load(in);

    // References
    //disassembler->references()->load(in);
    //return disassembler;
    return nullptr;
}

const String &Database::lastError() { return DatabaseImpl::m_lasterror; }

} // namespace REDasm
