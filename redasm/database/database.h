#pragma once

// struct RDBFile
// {
//     char signature[3];
//     u32 rdbbits;                   // Check size_t bits
//     u32 version;
//     String filename;          // XORified
//     String loaderid;
//     String assemblerid;
//
//     Buffer buffer;                 // ZLib compressed stream
//
//     struct {
//         SegmentList segments;
//         SymbolTable symboltable;
//         ListingItem items[];
//         u64 line, column;
//     } document;
//
//     ReferenceTable references;
// };

#define RDB_SIGNATURE        "RDB"
#define RDB_SIGNATURE_EXT    "rdb"
#define RDB_SIGNATURE_LENGTH 3
#define RDB_VERSION          u32(2)

#include "../types/string.h"

namespace REDasm {

class Disassembler;

class Database
{
    public:
        Database() = delete;
        static const String& lastError();
        static bool save(REDasm::Disassembler* disassembler, const String& dbfilename, const String& filename);
        static REDasm::Disassembler* load(const String& dbfilename, String& filename);
};

} // namespace REDasm
