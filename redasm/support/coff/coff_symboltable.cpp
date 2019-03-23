#include "coff_symboltable.h"
#include "../../plugins/loader.h"

#define	COFF_ENTRYSIZE    18
#define	COFF_AUXENTRYSIZE 18
#define COFF_NEXTENTRY(symentry) reinterpret_cast<const COFF_Entry*>(reinterpret_cast<const u8*>(symentry) + COFF_ENTRYSIZE + (symentry->e_numaux * COFF_AUXENTRYSIZE))
#define COFF_IS_FUNCTION(x)      (((x) & N_TMASK) == (IMAGE_SYM_DTYPE_FUNCTION << N_BTSHFT))

namespace REDasm {
namespace COFF {

SymbolTable::SymbolTable(const u8 *symdata, u64 count): m_count(count), m_symdata(symdata)
{
    m_stringtable = reinterpret_cast<const char*>(m_symdata + (count * COFF_ENTRYSIZE));
}

void SymbolTable::read(const SymbolCallback& symbolcb)
{
    const COFF_Entry* entry = reinterpret_cast<const COFF_Entry*>(m_symdata);
    std::string name;

    while(reinterpret_cast<const size_t*>(entry) < reinterpret_cast<const size_t*>(m_stringtable))
    {
        if((entry->e_scnum > 0) && COFF_IS_FUNCTION(entry->e_type) && ((entry->e_sclass == C_LABEL) || (entry->e_sclass == C_EXT) || (entry->e_sclass == C_STAT)))
        {
            if(!entry->e_zeroes)
                name = this->nameFromTable(entry->e_offset);
            else
                name = this->nameFromEntry(reinterpret_cast<const char*>(&entry->e_name));

            if(!name.empty())
                symbolcb(name, entry);
        }

        entry = COFF_NEXTENTRY(entry);
    }
}

const COFF_Entry *SymbolTable::at(u32 index) const
{
    if(index >= m_count)
        return nullptr;

    const COFF_Entry* entry = reinterpret_cast<const COFF_Entry*>(m_symdata);
    return &entry[index];
}

std::string SymbolTable::nameFromTable(u64 offset) const { return std::string(reinterpret_cast<const char*>(m_stringtable + offset)); }

std::string SymbolTable::nameFromEntry(const char *name) const
{
    size_t len = std::min(strlen(name), static_cast<size_t>(E_SYMNMLEN));
    return std::string(name, len);
}

void loadSymbols(const SymbolCallback& symbolcb, const u8 *symdata, u64 count)
{
    SymbolTable symtable(symdata, count);
    symtable.read(symbolcb);
}

const COFF_Entry *getSymbolAt(const u8 *symdata, u64 count, u32 idx)
{
    SymbolTable symtable(symdata, count);
    return symtable.at(idx);
}

} // namespace COFF
} // namespace REDasm
