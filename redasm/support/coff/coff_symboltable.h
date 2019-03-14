#ifndef COFF_SYMBOLTABLE_H
#define COFF_SYMBOLTABLE_H

#include <functional>
#include "coff_types.h"
#include "../../redasm.h"

namespace REDasm {
namespace COFF {

typedef std::function<void(const std::string&, COFF::COFF_Entry*)> SymbolCallback;

class SymbolTable
{
    friend class LoaderPlugin;

    public:
        SymbolTable(u8* symdata, u64 count);
        void read(const SymbolCallback &symbolcb);

    private:
        std::string nameFromTable(u64 offset) const;
        std::string nameFromEntry(const char* name) const;

    private:
        u64 m_count;
        u8* m_symdata;
        char* m_stringtable;
};

void loadSymbols(const SymbolCallback &symbolcb, u8* symdata, u64 count);

} // namespace COFF
} // namespace REDasm

#endif // COFF_SYMBOLTABLE_H
