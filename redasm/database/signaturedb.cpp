#include "signaturedb.h"
#include "../support/serializer.h"
#include "../support/utils.h"
#include "../support/hash.h"
#include "../redasm_api.h"
#include <cstring>
#include <fstream>
#include <array>

namespace REDasm {

SignatureDB::SignatureDB() { }

bool SignatureDB::load(const std::string &sigfilename)
{
    std::fstream ifs(sigfilename, std::ios::in | std::ios::binary);

    if(!ifs.is_open() || !Serializer::checkSignature(ifs, SDB_SIGNATURE))
        return false;

    u32 version = 0;
    Serializer::deserializeScalar(ifs, &version);

    if(version != SDB_VERSION)
        return false;

    Serializer::deserializeArray<std::list, Signature>(ifs, m_signatures, [&](Signature& sig) {
        Serializer::deserializeScalar(ifs, &sig.size);
        Serializer::deserializeScalar(ifs, &sig.symboltype);
        Serializer::deserializeString(ifs, sig.name);

        Serializer::deserializeArray<std::list, SignaturePattern>(ifs, sig.patterns, [&](SignaturePattern& sigpattern) {
            this->deserializePattern(ifs, sigpattern);
        });
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

    Serializer::serializeArray<std::list, Signature>(ofs, m_signatures, [&](const Signature& sig) {
        Serializer::serializeScalar(ofs, sig.size);
        Serializer::serializeScalar(ofs, sig.symboltype);
        Serializer::serializeString(ofs, sig.name);

        Serializer::serializeArray<std::list, SignaturePattern>(ofs, sig.patterns, [&](const SignaturePattern& sigpattern) {
            this->serializePattern(ofs, sigpattern);
        });
    });

    return true;
}

void SignatureDB::search(const BufferRef &br, const SignatureDB::SignatureFound &cb) const
{
    for(const Signature& sig : m_signatures)
    {
        if(sig.size != br.size())
            continue;

       this->searchSignature(br, sig, cb);
    }
}

SignatureDB &SignatureDB::operator <<(const Signature &signature) { m_signatures.push_back(signature); return *this; }

void SignatureDB::searchSignature(const BufferRef &br, const Signature &sig, const SignatureDB::SignatureFound &cb) const
{
    for(offset_t i = 0; i < br.size(); )
    {
        if(!this->checkPatterns(br, i, sig))
        {
            i++;
            continue;
        }

        cb(&sig);
        break;
    }
}

bool SignatureDB::checkPatterns(const BufferRef &br, offset_t offset, const Signature &sig) const
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
            if(Hash::crc16(br.data() + offset, pattern.size) != pattern.checksum)
                return false;

            offset += pattern.size;
            continue;
        }

        REDasm::log("ERROR: Unknown pattern type @ " + offset);
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
