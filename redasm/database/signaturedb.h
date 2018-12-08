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

/*
 * SignaturePattern valid fields:
 * - Byte: type, offset, byte,
 * - Checksum: type, size, checksum
 * - Skip: type, size
 */

namespace SignaturePatternType {

enum : u32 {
    First = 0,
    Byte, CheckSum, Skip,
    Last = Skip
};

}

struct SignatureSymbol
{
    std::string name;
    offset_t offset;
    u32 symboltype;
};

struct SignaturePattern
{
    SignaturePattern(): type(0), size(0), checksum(0) { }

    u32 type;

    union {
        u64 size;
        offset_t offset;
    };

    union {
        u8 byte;
        u16 checksum; // crc16
    };
};

struct Signature
{
    u64 size;
    SignaturePattern first, last; // Always type 'Byte'
    std::list<SignaturePattern> patterns;
    std::list<SignatureSymbol> symbols;
};

class SignatureDB
{
    public:
        typedef std::function<void(const SignatureSymbol&, offset_t)> SignatureFound;

    public:
        SignatureDB();
        bool load(const std::string& sigfilename);
        bool save(const std::string& sigfilename);

    public:
        void search(const BufferRef& br, const SignatureFound& cb) const;
        SignatureDB& operator <<(const Signature &signature);

    private:
        void searchSignature(const BufferRef& br, const Signature& sig, const SignatureFound& cb) const;
        bool checkPatterns(const BufferRef& br, offset_t offset, const Signature &sig) const;
        void serializePattern(std::fstream& ofs, const SignaturePattern& sigpattern) const;
        void serializeSymbol(std::fstream& ofs, const SignatureSymbol& sigsymbol) const;
        void deserializePattern(std::fstream& ifs, SignaturePattern& sigpattern) const;
        void deserializeSymbol(std::fstream& ofs, SignatureSymbol& sigsymbol) const;

    private:
        std::list<Signature> m_signatures;
};

} // namespace REDasm

#endif // SIGNATUREDB_H
