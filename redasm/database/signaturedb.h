#ifndef SIGNATUREDB_H
#define SIGNATUREDB_H

#define SDB_SIGNATURE_LENGTH 3
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

    SIGNATURE_FIELD(u64, offset)
    SIGNATURE_FIELD(u64, size)
    SIGNATURE_FIELD(u16, checksum) // CRC-16
};

struct Signature: public json
{
    Signature(): json() { (*this)["patterns"] = json::array(); }

    SIGNATURE_FIELD(u32, symboltype)
    SIGNATURE_FIELD(u64, size)
    SIGNATURE_FIELD(std::string, name)
    SIGNATURE_FIELD(std::string, assembler)

    json patterns() const { return (*this)["patterns"]; }
    void patterns(const json& p) { (*this)["patterns"] = p; }
};

class SignatureDB
{
    public:
        typedef std::function<void(const SignaturePattern&)> SignatureFound;

    public:
        SignatureDB();
        static bool isCompatible(const Signature& signature, const DisassemblerAPI *disassembler);
        void setName(const std::string& name);
        bool load(const std::string& sigfilename);
        bool save(const std::string& sigfilename);

    public:
        void search(const BufferView& view, const SignatureFound& cb) const;
        SignatureDB& operator <<(const Signature &signature);

    private:
        std::string uniqueAssembler(u32 idx) const;
        s32 uniqueAssemblerIdx(const Signature& signature) const;
        void pushUniqueAssembler(const Signature &signature);
        void searchSignature(const BufferView& view, const json& sig, const SignatureFound& cb) const;
        bool checkPatterns(const BufferView& view, offset_t offset, const Signature &sig) const;

    private:
        json m_json;
};

} // namespace REDasm

#endif // SIGNATUREDB_H
