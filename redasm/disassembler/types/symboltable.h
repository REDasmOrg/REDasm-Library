#pragma once

#include <redasm/libs/visit_struct/visit_struct.hpp>
#include <redasm/types/api.h>
#include "../../pimpl.h"

namespace REDasm {

enum class SymbolType: size_t {
    None               = 0,
    Data               = (1 << 0),
    String             = (1 << 1),
    Code               = (1 << 2),

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

class Symbol: public Object
{
    REDASM_OBJECT(Symbol)

    public:
        Symbol();
        Symbol(SymbolType type, tag_t tag, address_t address, const String& name);
        void lock();
        bool is(SymbolType t) const;
        bool isFunction() const;
        bool isImport() const;
        bool isExport() const;
        bool isEntryPoint() const;
        bool isLocked() const;

    public:
        void save(cereal::BinaryOutputArchive &a) const override;
        void load(cereal::BinaryInputArchive &a) override;

    public:
        SymbolType type;
        tag_t tag;
        address_t address;
        size_t size;
        String name;
};

class SymbolTableImpl;

class LIBREDASM_API SymbolTable: public Object
{
    REDASM_OBJECT(SymbolTable)
    PIMPL_DECLARE_P(SymbolTable)
    PIMPL_DECLARE_PRIVATE(SymbolTable)

    public:
        SymbolTable();
        tag_t size() const;
        bool create(address_t address, const String& name, SymbolType type, tag_t tag = 0);
        Symbol *symbol(address_t address) const;
        Symbol *symbol(const String& name) const;
        void iterate(SymbolType type, const std::function<bool(const Symbol*)> &cb) const;
        bool erase(address_t address);
        void clear();

    public:
        void save(cereal::BinaryOutputArchive &a) const override;
        void load(cereal::BinaryInputArchive &a) override;

    public:
        static String normalized(const String &s);
        static String name(address_t address, SymbolType type);
        static String name(address_t address, const String& s, SymbolType type);
        static String name(const String& name, address_t address);
};

} // namespace REDasm

VISITABLE_STRUCT(REDasm::Symbol, type, tag, address, size, name);
