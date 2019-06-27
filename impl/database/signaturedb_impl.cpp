#include "signaturedb_impl.h"
#include <redasm/plugins/loader/loader.h>
#include <redasm/plugins/assembler/assembler.h>
#include <redasm/disassembler/disassembler.h>
#include <redasm/support/utils.h>
#include <redasm/support/hash.h>
#include <redasm/context.h>
#include <cstring>
#include <fstream>

namespace REDasm {

SignatureDBImpl::SignatureDBImpl()
{
    m_json["version"] = SDB_VERSION;
    m_json["name"] = "Unknown Signature";
    m_json["assembler"] = String();
    m_json["signatures"] = json::array();
}

bool SignatureDBImpl::isCompatible(const Disassembler *disassembler) const { return m_json["assembler"] == disassembler->loader()->assembler().id; }

String SignatureDBImpl::assembler() const
{
    auto it = m_json.find("assembler");

    if(it != m_json.end())
        return *it;

    return String();
}

String SignatureDBImpl::name() const
{
    auto it = m_json.find("name");

    if(it != m_json.end())
        return *it;

    return String();
}

size_t SignatureDBImpl::size() const
{
    auto it = m_json.find("signatures");

    if(it != m_json.end())
        return it->size();

    return 0;
}

const json &SignatureDBImpl::at(size_t index) const { return m_json["signatures"][index]; }
void SignatureDBImpl::setAssembler(const String &assembler) { m_json["assembler"] = assembler; }
void SignatureDBImpl::setName(const String &name) { m_json["name"] = name; }

bool SignatureDBImpl::load(const String &sigfilename)
{
    std::ifstream ifs(sigfilename.c_str(), std::ios::in);

    if(!ifs.is_open())
        return false;

    ifs >> m_json;

    if(!m_json.contains("version"))
    {
        r_ctx->log("Missing 'version' field");
        return false;
    }

    if(m_json["version"] != SDB_VERSION)
    {
        r_ctx->log("Invalid version: Expected " + String(SDB_VERSION).quoted() + ", got " + String(static_cast<size_t>(m_json["version"])).quoted());
        return false;
    }

    return true;
}

bool SignatureDBImpl::save(const String &sigfilename)
{
    std::ofstream ofs(sigfilename.c_str(), std::ios::out | std::ios::trunc);

    if(!ofs.is_open())
        return false;

    ofs << m_json.dump(2);
    return true;
}

void SignatureDBImpl::search(const BufferView &view, const SignatureDB::SignatureFound &cb) const
{
    for(const json& sig : m_json["signatures"])
    {
        if(sig["size"] != view.size())
            continue;

       this->searchSignature(view, sig, cb);
    }
}

void SignatureDBImpl::searchSignature(const BufferView &view, const json &sig, const SignatureDB::SignatureFound &cb) const
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

bool SignatureDBImpl::checkPatterns(const BufferView &view, offset_t offset, const json &sig) const
{
    for(const json& pattern : sig["patterns"])
    {
        offset_t patternoffset = pattern["offset"];
        size_t patternsize = pattern["size"];
        u16 patternchecksum = pattern["checksum"];

        if(Hash::crc16(view.data() + patternoffset, patternsize) != patternchecksum)
            return false;
    }

    return true;
}

} // namespace REDasm
