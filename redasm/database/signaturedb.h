#ifndef SIGNATUREDB_H
#define SIGNATUREDB_H

#define SDB_SIGNATURE        "SDB"
#define SDB_SIGNATURE_EXT    "sdb"
#define SDB_VERSION          1

#include <string>
#include <deque>
#include "../redasm_types.h"

namespace REDasm {

/*
 * SignaturePattern valid fields:
 * - Byte: type, byte, offset
 * - Checksum: type, checksum, offset, size
 * - Skip: type, size
 */

enum SignaturePatternType: u32 {
    First = 0,
    Byte, CheckSum, Skip,
    Last = Skip
};

struct SignaturePattern
{
    SignaturePattern(): type(0) { seek.offset = value.checksum = 0; }

    u32 type;

    union {
        u8 byte;
        u16 checksum; // crc16
    } value;

    struct {
        u64 offset;
        u64 size;
    } seek;
};

struct Signature
{
    std::string name;
    u64 size;
    SignaturePattern first, last; // Always type 'Byte'
    std::deque<SignaturePattern> patterns;
};

class SignatureDB
{
    public:
        SignatureDB();
        bool load(const std::string& sigfilename);
        bool save(const std::string& sigfilename);
};

} // namespace REDasm

#endif // SIGNATUREDB_H
