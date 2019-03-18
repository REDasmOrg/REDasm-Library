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

namespace REDasm {

using json = nlohmann::json;

class DisassemblerAPI;

/*
 * SignaturePattern valid fields:
 * - Checksum: type, size, checksum
 * - Skip: type, size
 */

namespace SignaturePatternType {

enum : u32 {
    First = 0,
    CheckSum, Skip,
    Last = Skip
};

}

struct SignaturePattern
{
    SignaturePattern(): type(0), size(0), checksum(0) { }

    u32 type;
    u64 size;
    u16 checksum; // CRC-16
};

struct Signature
{
    u32 bits, symboltype;
    u64 size;
    std::string name, assembler;
    std::list<SignaturePattern> patterns;
};

class SignatureDB
{
    public:
        typedef std::function<void(const json&)> SignatureFound;

    public:
        SignatureDB();
        static bool isCompatible(const json &signature, const DisassemblerAPI *disassembler);
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
        bool checkPatterns(const BufferView& view, offset_t offset, const json &sig) const;

    private:
        json m_json;
};

} // namespace REDasm

#endif // SIGNATUREDB_H
