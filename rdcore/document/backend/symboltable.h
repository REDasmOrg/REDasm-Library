#pragma once

#include <unordered_map>
#include <map>
#include <set>
#include <string>
#include <rdapi/types.h>
#include <rdapi/symbol.h>
#include "../../containers/sortedcontainer.h"
#include "../../object.h"

class SymbolTable: public Object
{
    private:
        typedef std::function<bool(rd_address)> AddressCallback;
        typedef std::unordered_map<rd_address, std::string> StringTable;
        typedef std::unordered_map<std::string, rd_address> ByName;
        typedef std::unordered_map<rd_type, std::set<rd_address>> ByType;
        typedef std::map<rd_address, RDSymbol> ByAddress;

    public:
        SymbolTable(Context* ctx);
        const char* getName(rd_address address) const;
        bool get(rd_address address, RDSymbol* symbol) const;
        bool get(const char* name, RDSymbol* symbol) const;
        void remove(rd_address address);
        void each(const AddressCallback& cb) const;
        void eachType(rd_type type, const AddressCallback& cb) const;
        bool find(const std::string& q, RDSymbol* symbol, rd_type hint) const;
        bool findR(const std::string& q, RDSymbol* symbol, rd_type hint) const;
        size_t findAll(const std::string& q, const RDSymbol** symbols, rd_type hint) const;
        size_t findAllR(const std::string& q, const RDSymbol** symbols, rd_type hint) const;

    public:
        ByAddress::const_iterator begin() const;
        ByAddress::const_iterator end() const;

    public:
        void create(rd_address address, std::string name, rd_type type, rd_flag flags);
        bool rename(rd_address address, std::string newname);
        static std::string name(rd_address address, const std::string& s, rd_type type, rd_flag flags);
        static std::string name(rd_address address, rd_type type, rd_flag flags);
        static std::string name(rd_address address, const std::string& s);

    private:
        bool isSymbolAccepted(const std::string& q, rd_address address) const;
        bool findByType(const std::string& q, RDSymbol* symbol, rd_type type) const;
        size_t findAllByType(const std::string& q, const RDSymbol** symbols, rd_type type) const;

    private:
        static std::string name(const char* s, rd_address address);
        static std::string prefix(rd_type type, rd_flag flags);

    private:
        mutable std::vector<RDSymbol> m_result;
        StringTable m_stringtable;
        ByAddress m_byaddress;
        ByName m_byname;
        ByType m_bytype;
};
