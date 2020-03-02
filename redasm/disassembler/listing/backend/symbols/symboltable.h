#pragma once

#include "../../../../pimpl.h"
#include "symbol.h"

namespace REDasm {

class SymbolTableImpl;

class LIBREDASM_API SymbolTable: public Object
{
    REDASM_OBJECT(SymbolTable)
    PIMPL_DECLARE_P(SymbolTable)
    PIMPL_DECLARE_PRIVATE(SymbolTable)

    public:
        SymbolTable();
        size_t size() const;
        Symbol *get(address_t address) const;
        Symbol *get(const String& name) const;
        bool erase(address_t address);
        void clear();

   public:
        bool rename(address_t address, const String& newname);
        void create(address_t address, const String& name, type_t type, flag_t flags, tag_t tag = 0);
        void create(address_t address, const String& name, type_t type, tag_t tag = 0);
        void create(address_t address, type_t type, flag_t flags, tag_t tag = 0);
        void create(address_t address, type_t type, tag_t tag = 0);

    public:
        void save(cereal::BinaryOutputArchive &a) const override;
        void load(cereal::BinaryInputArchive &a) override;

    public:
        static String normalized(const String &s);
        static String name(address_t address, type_t type, flag_t flags = Symbol::T_None);
        static String name(address_t address, const String& s, type_t type, flag_t flags = Symbol::T_None);
        static String name(const String& name, address_t address);
};

} // namespace REDasm
