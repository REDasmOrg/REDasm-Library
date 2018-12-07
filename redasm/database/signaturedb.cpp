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

    //TODO: Load Signatures

    return true;
}

bool SignatureDB::save(const std::string &sigfilename)
{
    std::fstream ofs(sigfilename, std::ios::out | std::ios::trunc | std::ios::binary);

    if(!ofs.is_open())
        return false;

    Serializer::serializeScalar(ofs, SDB_VERSION, sizeof(u32));

    //TODO: Save Signatures

    return true;
}

} // namespace REDasm
