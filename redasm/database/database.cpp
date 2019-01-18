#include "database.h"
#include "../support/serializer.h"
#include "../plugins/assembler/assembler.h"
#include "../plugins/format.h"
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
    FormatPlugin* format = disassembler->format();
    ReferenceTable* references = disassembler->references();

    ofs.write(RDB_SIGNATURE, RDB_SIGNATURE_LENGTH);
    Serializer::serializeScalar(ofs, RDB_VERSION, sizeof(u32));
    Serializer::obfuscateString(ofs, filename);
    Serializer::serializeString(ofs, format->name());

    if(!Serializer::compressBuffer(ofs, format->buffer()))
    {
        m_lasterror = "Cannot compress database " + REDasm::quoted(dbfilename);
        return false;
    }

    document->serializeTo(ofs);
    references->serializeTo(ofs);
    return true;
}

Disassembler *Database::load(const std::string &dbfilename, std::string &filename)
{
    m_lasterror.clear();
    std::fstream ifs(dbfilename, std::ios::in | std::ios::binary);

    if(!ifs.is_open())
    {
        m_lasterror = "Cannot open " + REDasm::quoted(dbfilename);
        return NULL;
    }

    if(!Serializer::checkSignature(ifs, RDB_SIGNATURE))
    {
        m_lasterror = "Signature check failed for " + REDasm::quoted(dbfilename);
        return NULL;
    }

    u32 version = 0;
    Serializer::deserializeScalar(ifs, &version, sizeof(u32));

    if(version != RDB_VERSION)
    {
        m_lasterror = "Invalid version, got " + std::to_string(version) + " " + std::to_string(RDB_VERSION) + " required";
        return NULL;
    }

    Buffer buffer;
    std::string formatname;
    Serializer::deobfuscateString(ifs, filename);
    Serializer::deserializeString(ifs, formatname);

    if(!Serializer::decompressBuffer(ifs, buffer))
    {
        m_lasterror = "Cannot decompress database " + REDasm::quoted(dbfilename);
        return NULL;
    }

    std::unique_ptr<FormatPlugin> format(REDasm::getFormat(buffer));

    if(!format)
    {
        m_lasterror = "Unsupported format: " + REDasm::quoted(formatname);
        return NULL;
    }

    AssemblerPlugin* assembler = REDasm::getAssembler(format->assembler());

    if(!assembler)
    {
        m_lasterror = "Unsupported assembler: " + REDasm::quoted(format->assembler());
        return NULL;
    }

    auto& document = format->document();
    document->deserializeFrom(ifs);

    Disassembler* disassembler = new Disassembler(assembler, format.release());
    ReferenceTable* references = disassembler->references();
    references->deserializeFrom(ifs);
    return disassembler;
}

const std::string &Database::lastError() { return m_lasterror; }

} // namespace REDasm
