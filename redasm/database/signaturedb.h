#ifndef SIGNATUREDB_H
#define SIGNATUREDB_H

#define SDB_SIGNATURE        "SDB"
#define SDB_SIGNATURE_EXT    "sdb"
#define SDB_SIGNATURE_LENGTH 3
#define SDB_VERSION          1

#include <string>
#include <list>
#include "../redasm_types.h"

namespace REDasm {

/*
 * SignaturePattern valid fields:
 * - Byte: type, size = 1, byte,
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
    SignaturePattern(): type(0), size(0) { value.checksum = 0; }

    u32 type;
    u64 size;

    union {
        u8 byte;
        u16 checksum; // crc16
    } value;
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
        SignatureDB();
        bool load(const std::string& sigfilename);
        bool save(const std::string& sigfilename);

    public:
        SignatureDB& operator <<(const Signature &signature);

    private:
        void serializePattern(std::fstream& ofs, const SignaturePattern& sigpattern) const;
        void serializeSymbol(std::fstream& ofs, const SignatureSymbol& sigsymbol) const;
        void deserializePattern(std::fstream& ifs, SignaturePattern& sigpattern) const;
        void deserializeSymbol(std::fstream& ofs, SignatureSymbol& sigsymbol) const;

    private:
        std::list<Signature> m_signatures;
};

} // namespace REDasm

#endif // SIGNATUREDB_H
