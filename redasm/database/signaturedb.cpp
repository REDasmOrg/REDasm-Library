#include "signaturedb.h"
#include "../support/serializer.h"
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

SignatureDB &SignatureDB::operator <<(const Signature &signature) { m_signatures.push_back(signature); return *this; }

void SignatureDB::serializePattern(std::fstream &ofs, const SignaturePattern &sigpattern) const
{
    Serializer::serializeScalar(ofs, sigpattern.type);
    Serializer::serializeScalar(ofs, sigpattern.size);

    if(sigpattern.type == SignaturePatternType::Byte)
        Serializer::serializeScalar(ofs, sigpattern.value.byte);
    else if(sigpattern.type == SignaturePatternType::CheckSum)
        Serializer::serializeScalar(ofs, sigpattern.value.checksum);
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
    Serializer::deserializeScalar(ifs, &sigpattern.size);

    if(sigpattern.type == SignaturePatternType::Byte)
        Serializer::deserializeScalar(ifs, &sigpattern.value.byte);
    else if(sigpattern.type == SignaturePatternType::CheckSum)
        Serializer::deserializeScalar(ifs, &sigpattern.value.checksum);
}

void SignatureDB::deserializeSymbol(std::fstream &ifs, SignatureSymbol &sigsymbol) const
{
    Serializer::deserializeString(ifs, sigsymbol.name);
    Serializer::deserializeScalar(ifs, &sigsymbol.offset);
    Serializer::deserializeScalar(ifs, &sigsymbol.symboltype);
}

} // namespace REDasm
