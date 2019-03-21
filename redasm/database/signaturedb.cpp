#include "signaturedb.h"
#include "../plugins/loader.h"
#include "../plugins/assembler/assembler.h"
#include "../support/serializer.h"
#include "../support/utils.h"
#include "../support/hash.h"
#include "../redasm_api.h"
#include <cstring>
#include <fstream>
#include <array>

namespace REDasm {

SignatureDB::SignatureDB()
{
    m_json["version"] = SDB_VERSION;
    m_json["name"] = "Unknown Signature";
    m_json["assembler"] = std::string();
    m_json["signatures"] = json::array();
}

bool SignatureDB::isCompatible(const DisassemblerAPI *disassembler) { return m_json["assembler"] == disassembler->loader()->assembler(); }
std::string SignatureDB::name() const { return m_json["name"]; }
void SignatureDB::setAssembler(const std::string &assembler) { m_json["assembler"] = assembler; }
void SignatureDB::setName(const std::string &name) { m_json["name"] = name; }
bool SignatureDB::load(const std::string &sigfilename)
{
    std::ifstream ifs(sigfilename, std::ios::in);

    if(!ifs.is_open())
        return false;

    ifs >> m_json;

    if(!m_json.contains("version"))
    {
        REDasm::log("Missing 'version' field");
        return false;
    }

    if(m_json["version"] != SDB_VERSION)
    {
        REDasm::log("Invalid version: Expected " + REDasm::quoted(SDB_VERSION) + ", got " + REDasm::quoted(static_cast<size_t>(m_json["version"])));
        return false;
    }

    return true;
}

bool SignatureDB::save(const std::string &sigfilename)
{
    std::ofstream ofs(sigfilename, std::ios::out | std::ios::trunc);

    if(!ofs.is_open())
        return false;

    ofs << m_json.dump(2);
    return true;
}

void SignatureDB::search(const BufferView &view, const SignatureDB::SignatureFound &cb) const
{
    for(const json& sig : m_json["signatures"])
    {
        if(sig["size"] != view.size())
            continue;

       this->searchSignature(view, sig, cb);
    }
}

SignatureDB &SignatureDB::operator <<(const Signature &signature)
{
    m_json["signatures"].push_back(signature);
    return *this;
}

void SignatureDB::searchSignature(const BufferView &view, const json &sig, const SignatureDB::SignatureFound &cb) const
{
    for(offset_t i = 0; i < view.size(); )
    {
        if(!this->checkPatterns(view, i, sig))
        {
            i++;
            continue;
        }

        cb(sig);
        break;
    }
}

bool SignatureDB::checkPatterns(const BufferView &view, offset_t offset, const json &sig) const
{
    for(const json& pattern : sig["patterns"])
    {
        offset_t patternoffset = pattern["offset"];
        u64 patternsize = pattern["size"];
        u16 patternchecksum = pattern["checksum"];

        if(Hash::crc16(view.data() + patternoffset, patternsize) != patternchecksum)
            return false;
    }

    return true;
}

} // namespace REDasm
