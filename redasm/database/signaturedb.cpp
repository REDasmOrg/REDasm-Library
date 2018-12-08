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
        this->deserializePattern(ifs, sig.first);
        this->deserializePattern(ifs, sig.last);

        Serializer::deserializeArray<std::list, SignaturePattern>(ifs, sig.patterns, [&](SignaturePattern& sigpattern) {
            this->deserializePattern(ifs, sigpattern);
        });

        Serializer::deserializeArray<std::list, SignatureSymbol>(ifs, sig.symbols, [&](SignatureSymbol& sigsymbol) {
            this->deserializeSymbol(ifs, sigsymbol);
        });
    });

    return true;
}

bool SignatureDB::save(const std::string &sigfilename)
{
    std::fstream ofs(sigfilename, std::ios::out | std::ios::trunc | std::ios::binary);

    if(!ofs.is_open())
        return false;

    ofs.write(SDB_SIGNATURE, SDB_SIGNATURE_LENGTH);
    Serializer::serializeScalar(ofs, SDB_VERSION, sizeof(u32));

    Serializer::serializeArray<std::list, Signature>(ofs, m_signatures, [&](const Signature& sig) {
        Serializer::serializeScalar(ofs, sig.size);
        this->serializePattern(ofs, sig.first);
        this->serializePattern(ofs, sig.last);

        Serializer::serializeArray<std::list, SignaturePattern>(ofs, sig.patterns, [&](const SignaturePattern& sigpattern) {
            this->serializePattern(ofs, sigpattern);
        });

        Serializer::serializeArray<std::list, SignatureSymbol>(ofs, sig.symbols, [&](const SignatureSymbol& sigsymbol) {
            this->serializeSymbol(ofs, sigsymbol);
        });
    });

    return true;
}

void SignatureDB::search(const BufferRef &br, const SignatureDB::SignatureFound &cb) const
{
    for(const Signature& sig : m_signatures)
    {
        if(sig.size > br.size())
            continue;

       this->searchSignature(br, sig, cb);
    }
}

SignatureDB &SignatureDB::operator <<(const Signature &signature) { m_signatures.push_back(signature); return *this; }

void SignatureDB::searchSignature(const BufferRef &br, const Signature &sig, const SignatureDB::SignatureFound &cb) const
{
    for(offset_t i = 0; i < br.size(); )
    {
        if((i + sig.size) >= br.size())
            break;

        if((br[i + sig.first.offset] != sig.first.byte) || (br[i + sig.last.offset] != sig.last.byte))
        {
            i++;
            continue;
        }

        if(!this->checkPatterns(br, i, sig))
        {
            i++;
            continue;
        }

        for(const SignatureSymbol& sigsymbol : sig.symbols)
            cb(sigsymbol, i + sigsymbol.offset);

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

        if(pattern.type == SignaturePatternType::Byte)
        {
            if(br[offset] != pattern.byte)
                return false;

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

    if(sigpattern.type == SignaturePatternType::Byte)
    {
        Serializer::serializeScalar(ofs, sigpattern.offset);
        Serializer::serializeScalar(ofs, sigpattern.byte);
        return;
    }

    Serializer::serializeScalar(ofs, sigpattern.size);

    if(sigpattern.type == SignaturePatternType::CheckSum)
        Serializer::serializeScalar(ofs, sigpattern.checksum);
}

void SignatureDB::serializeSymbol(std::fstream &ofs, const SignatureSymbol &sigsymbol) const
{
    Serializer::serializeString(ofs, sigsymbol.name);
    Serializer::serializeScalar(ofs, sigsymbol.offset);
    Serializer::serializeScalar(ofs, sigsymbol.symboltype);
}

void SignatureDB::deserializePattern(std::fstream &ifs, SignaturePattern &sigpattern) const
{
    Serializer::deserializeScalar(ifs, &sigpattern.type);

    if(sigpattern.type == SignaturePatternType::Byte)
    {
        Serializer::deserializeScalar(ifs, &sigpattern.offset);
        Serializer::deserializeScalar(ifs, &sigpattern.byte);
        return;
    }

    Serializer::deserializeScalar(ifs, &sigpattern.size);

    if(sigpattern.type == SignaturePatternType::CheckSum)
        Serializer::deserializeScalar(ifs, &sigpattern.checksum);
}

void SignatureDB::deserializeSymbol(std::fstream &ifs, SignatureSymbol &sigsymbol) const
{
    Serializer::deserializeString(ifs, sigsymbol.name);
    Serializer::deserializeScalar(ifs, &sigsymbol.offset);
    Serializer::deserializeScalar(ifs, &sigsymbol.symboltype);
}

} // namespace REDasm
