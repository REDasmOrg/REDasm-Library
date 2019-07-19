#pragma once

#define RDB_SIGNATURE_ARRAY  { 'R', 'D', 'B' }
#define RDB_SIGNATURE        "RDB"
#define RDB_SIGNATURE_LENGTH 3

#define RDB_VERSION          3

/*
 * struct RDBFile
 * {
 *     struct {
 *         char signature[3];
 *         u32 version;
 *         u32 rdbbits;       // Check size_t bits
 *     } rdbheader;
 *
 *     String filename;       // XORified
 *     String loaderid;
 *     String assemblerid;
 *
 *     Buffer buffer;         // ZLib compressed stream
 *
 *     struct {
 *         address_t entry;
 *         SegmentList segments;
 *         SymbolTable symboltable;
 *         ListingItem items[];
 *         ListingCursor cursor;
 *     } document;
 *
 *     ReferenceTable references;
 * };
 */

#include <redasm/types/string.h>

namespace REDasm {

struct RDBHeader
{
    char signature[RDB_SIGNATURE_LENGTH];
    u32 version;
    u32 bits;
};

class DatabaseImpl
{
    public:
        DatabaseImpl() = delete;

    public:
        static String m_lasterror;
        static RDBHeader m_header;
};

} // namespace REDasm
