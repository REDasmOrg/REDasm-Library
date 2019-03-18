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
    m_json["name"] = "Unknown Signature";
    m_json["version"] = SDB_VERSION;
    m_json["assemblers"] = json::array();
    m_json["signatures"] = json::array();
}

bool SignatureDB::isCompatible(const json& signature, const DisassemblerAPI *disassembler)
{
    if(signature["assembler"] != disassembler->loader()->assembler())
        return false;

    if(signature["bits"] != disassembler->assembler()->bits())
        return false;

    return true;
}

void SignatureDB::setName(const std::string &name) { m_json["name"] = name; }
bool SignatureDB::load(const std::string &sigfilename)
{
    std::ifstream ifs(sigfilename, std::ios::in);

    if(!ifs.is_open())
        return false;

    ifs >> m_json;

    if(m_json["version"] != SDB_VERSION)
        return false;

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
    for(const auto& sig : m_json["signatures"])
    {
        if(sig["size"] != view.size())
            continue;

       this->searchSignature(view, sig, cb);
    }
}

SignatureDB &SignatureDB::operator <<(const Signature &signature)
{
    this->pushUniqueAssembler(signature);

    json signatureobj = json::object();
    signatureobj["bits"] = signature.bits;
    signatureobj["symboltype"] = signature.symboltype;
    signatureobj["size"] = signature.size;
    signatureobj["name"] = signature.name;
    signatureobj["assembler"] = signature.assembler;
    signatureobj["patterns"] = json::array();

    for(const auto& pattern : signature.patterns)
    {
        json patternobj = json::object();
        patternobj["type"] = pattern.type;
        patternobj["size"] = pattern.size;
        patternobj["checksum"] = pattern.checksum;

        signatureobj["patterns"].push_back(patternobj);
    }

    m_json["signatures"].push_back(signatureobj);
    return *this;
}

std::string SignatureDB::uniqueAssembler(u32 idx) const
{
    if(idx >= m_json["assemblers"].size())
        return std::string();

    return m_json["assemblers"][idx];
}

s32 SignatureDB::uniqueAssemblerIdx(const Signature &signature) const
{
    const auto& assemblers = m_json["assemblers"];

    for(s32 i = 0; i < assemblers.size(); i++)
    {
        if(assemblers[i] == signature.assembler)
            return i;
    }

    return -1;
}

void SignatureDB::pushUniqueAssembler(const Signature& signature)
{
    if(this->uniqueAssemblerIdx(signature) > -1)
        return;

    m_json["assemblers"].push_back(signature.assembler);
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
        if(pattern["type"] == SignaturePatternType::Skip)
        {
            offset += static_cast<offset_t>(pattern["size"]);
            continue;
        }

        if(pattern["type"] == SignaturePatternType::CheckSum)
        {
            if(Hash::crc16(&view + offset, pattern["size"]) != pattern["checksum"])
                return false;

            offset += static_cast<offset_t>(pattern["size"]);
            continue;
        }

        REDasm::log("ERROR: Unknown pattern type @ " + REDasm::hex(offset));
        return false;
    }

    return true;
}

} // namespace REDasm
