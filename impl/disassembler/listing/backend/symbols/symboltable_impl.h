#pragma once

#include <redasm/disassembler/listing/backend/symbols/symboltable.h>
#include <redasm/pimpl.h>
#include <unordered_map>

namespace REDasm {

typedef std::unique_ptr<Symbol> SymbolPtr;

class SymbolTableImpl
{
    PIMPL_DECLARE_Q(SymbolTable)
    PIMPL_DECLARE_PUBLIC(SymbolTable)

    public:
        typedef std::unordered_map<address_t, SymbolPtr> SymbolsByAddress;
        typedef std::unordered_map<String, address_t> SymbolsByName;

    public:
        SymbolTableImpl() = default;
        void save(cereal::BinaryOutputArchive &a) const;
        void load(cereal::BinaryInputArchive &a);
        bool rename(address_t address, const String& newname);
        static String prefix(type_t type, flag_t flags);

    private:
        SymbolsByAddress m_byaddress;
        SymbolsByName m_byname;
};

} // namespace REDasm
