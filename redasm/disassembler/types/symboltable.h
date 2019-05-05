#pragma once

#include <unordered_map>
#include "../../support/serializer.h"
#include "../../redasm.h"

namespace REDasm {

enum class SymbolType: u32 {
    None               = 0x00000000,
    Data               = 0x00000001, String = 0x00000002, Code = 0x00000004,

    Function           = 0x00000100 | Code,
    EntryPoint         = 0x00001000 | Function,
    Import             = 0x00002000 | Data,
    ExportData         = 0x00004000 | Data,
    ExportFunction     = 0x00008000 | Function,
    WideString         = 0x01000000 | String,
    Pointer            = 0x02000000,
    Locked             = 0x10000000,

    TableItem          = 0x00010000 | Pointer | Data,

    LockedMask         = ~Locked,
    FunctionMask       = Function                      & ~(Code      | Locked),
    ExportMask         = (ExportData | EntryPoint | ExportFunction) & ~(Function  | Data | Locked),
    ImportMask         = Import                        & ~(Data      | Locked),
    EntryPointMask     = EntryPoint                    & ~(Function),
    StringMask         = String                        & ~(Pointer),
    WideStringMask     = WideString                    & ~(String    | Pointer),
    TableItemMask      = TableItem                     & ~(Pointer   | Data),
};

ENUM_FLAGS_OPERATORS(SymbolType)

struct Symbol
{
    Symbol(): type(SymbolType::None), tag(0), address(0), size(0) { }
    Symbol(SymbolType type, u32 tag, address_t address, const std::string& name): type(type), tag(tag), address(address), size(0), name(name) { }
    void lock() { type |= SymbolType::Locked; }

    SymbolType type;
    tag_t tag;
    address_t address;
    u64 size;
    std::string name;

    constexpr bool is(SymbolType t) const { return type & t; }
    constexpr bool isFunction() const { return type & SymbolType::FunctionMask; }
    constexpr bool isImport() const { return type & SymbolType::ImportMask; }
    constexpr bool isLocked() const { return type & SymbolType::Locked; }
};

typedef std::unique_ptr<Symbol> SymbolPtr;

class SymbolTable
{
    private:
        typedef std::unordered_map<address_t, SymbolPtr> SymbolsByAddress;
        typedef std::unordered_map<std::string, address_t> SymbolsByName;

    public:
        SymbolTable() = default;
        u64 size() const;
        bool create(address_t address, const std::string& name, SymbolType type, tag_t tag = 0);
        Symbol *symbol(address_t address) const;
        Symbol *symbol(const std::string& name) const;
        void iterate(SymbolType type, const std::function<bool(const Symbol*)> &cb) const;
        bool erase(address_t address);
        void clear();

    public:
        static std::string normalized(std::string s);
        static std::string name(address_t address, SymbolType type);
        static std::string name(address_t address, const std::string& s, SymbolType type);

    private:
        static std::string prefix(SymbolType type);

    private:
        SymbolsByAddress m_byaddress;
        SymbolsByName m_byname;

    friend class Serializer<SymbolTable>;
};

template<> struct Serializer<SymbolTable> {
    static void write(std::fstream& fs, const SymbolTable* st) {
        Serializer<SymbolTable::SymbolsByAddress>::write(fs, st->m_byaddress);
        Serializer<SymbolTable::SymbolsByName>::write(fs, st->m_byname);
    }

    static void read(std::fstream& fs, SymbolTable* st) {
        Serializer<SymbolTable::SymbolsByAddress>::read(fs, st->m_byaddress);
        Serializer<SymbolTable::SymbolsByName>::read(fs, st->m_byname);
    }
};

} // namespace REDasm

VISITABLE_STRUCT(REDasm::Symbol, type, tag, address, size, name);
