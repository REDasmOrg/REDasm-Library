#pragma once

#include <unordered_map>
#include <string>
#include <rdapi/types.h>
#include <rdapi/symbol.h>
#include "../../containers/sortedcontainer.h"
#include "../../object.h"

class SymbolTable: public Object
{
    public:
        SymbolTable(Context* ctx);
        size_t size() const;
        const char* getName(rd_address address) const;
        bool get(rd_address address, RDSymbol* symbol) const;
        bool get(const char* name, RDSymbol* symbol) const;
        bool at(size_t idx, RDSymbol* symbol) const;
        void remove(rd_address address);

   public:
        void create(rd_address address, std::string name, rd_type type, rd_flag flags);
        bool rename(rd_address address, std::string newname);
        static std::string name(rd_address address, const char* s, rd_type type, rd_flag flags);
        static std::string name(rd_address address, rd_type type, rd_flag flags);

   private:
        static std::string name(const char* s, rd_address address);
        static std::string prefix(rd_type type, rd_flag flags);

    private:
        SortedContainer<rd_address, std::less<rd_address>, std::equal_to<rd_address>, true> m_addresses;
        std::unordered_map<rd_address, RDSymbol> m_byaddress;      // address   -> RDSymbol
        std::unordered_map<std::string, rd_address> m_byname;      // sym_xyz   -> address
        std::unordered_map<rd_address, std::string> m_stringtable; // address_t -> sym_xyz
};

