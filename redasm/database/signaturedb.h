#ifndef SIGNATUREDB_H
#define SIGNATUREDB_H

#define SDB_SIGNATURE        "SDB"
#define SDB_SIGNATURE_EXT    "sdb"
#define SDB_SIGNATURE_LENGTH 3
#define SDB_VERSION          1

#include <functional>
#include <string>
#include <list>
#include "../redasm_buffer.h"
#include "../redasm_types.h"

namespace REDasm {

class FormatPlugin;

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

    bool isCompatible(const FormatPlugin* format) const;
};

class SignatureDB
{
    public:
        typedef std::function<void(const Signature*)> SignatureFound;

    public:
        SignatureDB();
        bool load(const std::string& sigfilename);
        bool save(const std::string& sigfilename);

    public:
        void search(const BufferRef& br, const SignatureFound& cb) const;
        SignatureDB& operator <<(const Signature &signature);

    private:
        std::string uniqueAssembler(u32 idx) const;
        s32 uniqueAssemblerIdx(const Signature& signature) const;
        void pushUniqueAssembler(const Signature &signature);
        void searchSignature(const BufferRef& br, const Signature& sig, const SignatureFound& cb) const;
        bool checkPatterns(const BufferRef& br, offset_t offset, const Signature &sig) const;
        void serializePattern(std::fstream& ofs, const SignaturePattern& sigpattern) const;
        void deserializePattern(std::fstream& ifs, SignaturePattern& sigpattern) const;

    private:
        std::list<Signature> m_signatures;
        std::vector<std::string> m_assemblers;
};

} // namespace REDasm

#endif // SIGNATUREDB_H
