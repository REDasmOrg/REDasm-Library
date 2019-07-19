#pragma once

#define RDB_SIGNATURE_EXT "rdb"

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
