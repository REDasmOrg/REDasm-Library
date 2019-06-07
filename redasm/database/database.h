#pragma once

// struct RDBFile
// {
//     char signature[3];
//     u32 rdbbits;                   // Check size_t bits
//     u32 version;
//     std::string filename;          // XORified
//     std::string loaderid;
//     std::string assemblerid;
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

#include <string>

namespace REDasm {

class Disassembler;

class Database
{
    public:
        Database() = delete;
        static const std::string& lastError();
        static bool save(REDasm::Disassembler* disassembler, const std::string& dbfilename, const std::string& filename);
        static REDasm::Disassembler* load(const std::string& dbfilename, std::string& filename);
};

} // namespace REDasm
