#pragma once

#include <unordered_map>
#include <redasm/types/api_types.h>
#include "../../support/serializer.h"
#include "../../pimpl.h"

namespace REDasm {

enum class SymbolType: size_t {
    None               = 0,
    Data               = 1 << 0, String = 1 << 1, Code = 1 << 2,

    Function           = (1 << 3) | Code,
    EntryPoint         = (1 << 4) | Function,
    Import             = (1 << 5) | Data,
    ExportData         = (1 << 6) | Data,
    ExportFunction     = (1 << 7) | Function,
    WideString         = (1 << 8) | String,
    Pointer            = (1 << 9),
    Locked             = (1 << 10),

    TableItem          = (1 << 11) | Pointer | Data,

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
    Symbol(SymbolType type, tag_t tag, address_t address, const std::string& name): type(type), tag(tag), address(address), size(0), name(name) { }
    void lock() { type |= SymbolType::Locked; }

    SymbolType type;
    tag_t tag;
    address_t address;
    size_t size;
    std::string name;

    constexpr bool is(SymbolType t) const { return type & t; }
    constexpr bool isFunction() const { return type & SymbolType::FunctionMask; }
    constexpr bool isImport() const { return type & SymbolType::ImportMask; }
    constexpr bool isLocked() const { return type & SymbolType::Locked; }
};

class SymbolTableImpl;

class LIBREDASM_API SymbolTable
{
    PIMPL_DECLARE_P(SymbolTable)
    PIMPL_DECLARE_PRIVATE(SymbolTable)

    public:
        SymbolTable();
        tag_t size() const;
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
        static std::string name(const std::string& name, address_t address);

    friend struct Serializer<SymbolTable>;
};

template<> struct Serializer<SymbolTable> {
    static void write(std::fstream& fs, const SymbolTable* st);
    static void read(std::fstream& fs, SymbolTable* st);
};

} // namespace REDasm

VISITABLE_STRUCT(REDasm::Symbol, type, tag, address, size, name);
