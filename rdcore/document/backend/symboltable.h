#pragma once

#include <unordered_map>
#include <string>
#include <rdapi/types.h>
#include <rdapi/symbol.h>
#include "../../object.h"

class SymbolTable: public Object
{
    public:
        SymbolTable() = default;
        size_t size() const;
        const char* getName(address_t address) const;
        bool get(address_t address, RDSymbol* symbol) const;
        bool get(const char* name, RDSymbol* symbol) const;
        void remove(address_t address);

   public:
        void create(address_t address, const std::string& name, type_t type, flag_t flags);
        bool rename(address_t address, const std::string& newname);
        static std::string name(address_t address, type_t type, flag_t flags);

   private:
        static std::string normalized(const char* s);
        static std::string name(address_t address, const char* s, type_t type, flag_t flags);
        static std::string name(const char* s, address_t address);
        static std::string prefix(type_t type, flag_t flags);

    private:
        std::unordered_map<address_t, RDSymbol> m_byaddress;      // address   -> RDSymbol
        std::unordered_map<std::string, address_t> m_byname;      // sym_xyz   -> address
        std::unordered_map<address_t, std::string> m_stringtable; // address_t -> sym_xyz
};

