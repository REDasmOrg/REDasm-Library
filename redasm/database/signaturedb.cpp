#include "signaturedb.h"
#include "../plugins/format.h"
#include "../support/serializer.h"
#include "../support/utils.h"
#include "../support/hash.h"
#include "../redasm_api.h"
#include <cstring>
#include <fstream>
#include <array>

namespace REDasm {

bool Signature::isCompatible(const FormatPlugin *format) const
{
    if(this->assembler != format->assembler())
        return false;

    if(this->bits != format->bits())
        return false;

    return true;
}

bool SignatureDB::load(const std::string &sigfilename)
{
    std::fstream ifs(sigfilename, std::ios::in | std::ios::binary);

    if(!ifs.is_open() || !Serializer::checkSignature(ifs, SDB_SIGNATURE))
        return false;

    u32 version = 0;
    Serializer::deserializeScalar(ifs, &version);

    if(version != SDB_VERSION)
        return false;

    Serializer::deserializeArray<std::deque, std::string>(ifs, m_assemblers, [&](std::string& assembler) {
        Serializer::deserializeString(ifs, assembler);
    });

    Serializer::deserializeArray<std::list, Signature>(ifs, m_signatures, [&](Signature& signature) {
        u32 assembleridx = -1;

        Serializer::deserializeScalar(ifs, &assembleridx);
        Serializer::deserializeScalar(ifs, &signature.bits);
        Serializer::deserializeScalar(ifs, &signature.symboltype);
        Serializer::deserializeScalar(ifs, &signature.size);
        Serializer::deserializeString(ifs, signature.name);

        Serializer::deserializeArray<std::list, SignaturePattern>(ifs, signature.patterns, [&](SignaturePattern& sigpattern) {
            this->deserializePattern(ifs, sigpattern);
        });

        signature.assembler = this->uniqueAssembler(assembleridx);

        if(signature.assembler.empty())
            REDasm::log("Invalid assembler for " + REDasm::quoted(signature.name));
    });

    return true;
}

bool SignatureDB::save(const std::string &sigfilename)
{
    std::fstream ofs(sigfilename, std::ios::out | std::ios::binary | std::ios::trunc);

    if(!ofs.is_open())
        return false;

    ofs.write(SDB_SIGNATURE, SDB_SIGNATURE_LENGTH);
    Serializer::serializeScalar(ofs, SDB_VERSION, sizeof(u32));

    Serializer::serializeArray<std::deque, std::string>(ofs, m_assemblers, [&](const std::string& assembler) {
        Serializer::serializeString(ofs, assembler);
    });

    Serializer::serializeArray<std::list, Signature>(ofs, m_signatures, [&](const Signature& signature) {
        Serializer::serializeScalar(ofs, this->uniqueAssemblerIdx(signature));
        Serializer::serializeScalar(ofs, signature.bits);
        Serializer::serializeScalar(ofs, signature.symboltype);
        Serializer::serializeScalar(ofs, signature.size);
        Serializer::serializeString(ofs, signature.name);

        Serializer::serializeArray<std::list, SignaturePattern>(ofs, signature.patterns, [&](const SignaturePattern& sigpattern) {
            this->serializePattern(ofs, sigpattern);
        });
    });

    return true;
}

void SignatureDB::search(const BufferView &view, const SignatureDB::SignatureFound &cb) const
{
    for(const Signature& sig : m_signatures)
    {
        if(sig.size != view.size())
            continue;

       this->searchSignature(view, sig, cb);
    }
}

SignatureDB &SignatureDB::operator <<(const Signature &signature)
{
    this->pushUniqueAssembler(signature);
    m_signatures.push_back(signature);
    return *this;
}

std::string SignatureDB::uniqueAssembler(u32 idx) const
{
    if(idx >= m_assemblers.size())
        return std::string();

    return m_assemblers[idx];
}

s32 SignatureDB::uniqueAssemblerIdx(const Signature &signature) const
{
    for(s32 i = 0; i < m_assemblers.size(); i++)
    {
        if(m_assemblers[i] == signature.assembler)
            return i;
    }

    return -1;
}

void SignatureDB::pushUniqueAssembler(const Signature& signature)
{
    if(this->uniqueAssemblerIdx(signature) > -1)
        return;

    m_assemblers.push_back(signature.assembler);
}

void SignatureDB::searchSignature(const BufferView &view, const Signature &sig, const SignatureDB::SignatureFound &cb) const
{
    for(offset_t i = 0; i < view.size(); )
    {
        if(!this->checkPatterns(view, i, sig))
        {
            i++;
            continue;
        }

        cb(&sig);
        break;
    }
}

bool SignatureDB::checkPatterns(const BufferView &view, offset_t offset, const Signature &sig) const
{
    for(const SignaturePattern& pattern : sig.patterns)
    {
        if(pattern.type == SignaturePatternType::Skip)
        {
            offset += pattern.size;
            continue;
        }

        if(pattern.type == SignaturePatternType::CheckSum)
        {
            if(Hash::crc16(&view + offset, pattern.size) != pattern.checksum)
                return false;

            offset += pattern.size;
            continue;
        }

        REDasm::log("ERROR: Unknown pattern type @ " + REDasm::hex(offset));
        return false;
    }

    return true;
}

void SignatureDB::serializePattern(std::fstream &ofs, const SignaturePattern &sigpattern) const
{
    Serializer::serializeScalar(ofs, sigpattern.type);
    Serializer::serializeScalar(ofs, sigpattern.size);

    if(sigpattern.type == SignaturePatternType::CheckSum)
        Serializer::serializeScalar(ofs, sigpattern.checksum);
}

void SignatureDB::deserializePattern(std::fstream &ifs, SignaturePattern &sigpattern) const
{
    Serializer::deserializeScalar(ifs, &sigpattern.type);
    Serializer::deserializeScalar(ifs, &sigpattern.size);

    if(sigpattern.type == SignaturePatternType::CheckSum)
        Serializer::deserializeScalar(ifs, &sigpattern.checksum);
}

} // namespace REDasm
