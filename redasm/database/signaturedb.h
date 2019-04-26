#pragma once

#define SDB_VERSION          1

#include <functional>
#include <string>
#include <list>
#include <json.hpp>
#include "../types/buffer/bufferview.h"
#include "../types/base_types.h"

#define SIGNATURE_FIELD(T, name) T name() const { return (*this)[#name]; } \
                                 void name(const T& v) { (*this)[#name] = v; }

namespace REDasm {

using json = nlohmann::json;
class DisassemblerAPI;

struct SignaturePattern: public json {
    SignaturePattern(): json() { }

    SIGNATURE_FIELD(offset_t, offset)
    SIGNATURE_FIELD(u64, size)
    SIGNATURE_FIELD(u16, checksum) // CRC-16
};

struct Signature: public json
{
    Signature(): json() { (*this)["patterns"] = json::array(); }

    SIGNATURE_FIELD(u32, symboltype)
    SIGNATURE_FIELD(u64, size)
    SIGNATURE_FIELD(std::string, name)

    json& patterns() { return (*this)["patterns"]; }
    const json& patterns() const { return (*this)["patterns"]; }
    void patterns(const json& p) { (*this)["patterns"] = p; }
};

class SignatureDB
{
    public:
        typedef std::function<void(const json&)> SignatureFound;

    public:
        SignatureDB();
        bool isCompatible(const DisassemblerAPI *disassembler);
        std::string assembler() const;
        std::string name() const;
        u64 size() const;
        const json& at(u64 index) const;
        void setAssembler(const std::string& assembler);
        void setName(const std::string& name);
        bool load(const std::string& sigfilename);
        bool save(const std::string& sigfilename);
        void search(const BufferView& view, const SignatureFound& cb) const;
        SignatureDB& operator <<(const Signature &signature);

    private:
        void searchSignature(const BufferView& view, const json &sig, const SignatureFound& cb) const;
        bool checkPatterns(const BufferView& view, offset_t offset, const json &sig) const;

    private:
        json m_json;
};

} // namespace REDasm
