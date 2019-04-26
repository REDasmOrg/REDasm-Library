#pragma once

// http://wiki.osdev.org/COFF#Symbol_Table

#include <functional>
#include "coff_types.h"
#include "../../redasm.h"

namespace REDasm {
namespace COFF {

typedef std::function<void(const std::string&, const COFF::COFF_Entry*)> SymbolCallback;

class SymbolTable
{
    friend class LoaderPlugin;

    public:
        SymbolTable(const u8* symdata, u64 count);
        void read(const SymbolCallback &symbolcb);
        const COFF_Entry* at(u32 index) const;

    private:
        std::string nameFromTable(u64 offset) const;
        std::string nameFromEntry(const char* name) const;

    private:
        u64 m_count;
        const u8* m_symdata;
        const char* m_stringtable;
};

void loadSymbols(const SymbolCallback &symbolcb, const u8 *symdata, u64 count);
const COFF_Entry* getSymbolAt(const u8* symdata, u64 count, u32 idx);

} // namespace COFF
} // namespace REDasm
