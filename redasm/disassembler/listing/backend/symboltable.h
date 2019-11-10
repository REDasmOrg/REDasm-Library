#pragma once

#include <redasm/libs/visit_struct/visit_struct.hpp>
#include <redasm/types/api.h>
#include "../../../pimpl.h"

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

    DataNew     = 0x10000000,
    StringNew   = 0x10000001,
    LabelNew    = 0x10000002,
    FunctionNew = 0x10000004,
    ImportNew   = 0x10000005,
};

enum class SymbolFlags: size_t
{
    None         = 0,
    Weak         = (1 << 0),
    Export       = (1 << 1),
    EntryPoint   = (1 << 2),
    AsciiString  = (1 << 3),
    WideString   = (1 << 4),
    Pointer      = (1 << 5),
    TableItem    = (1 << 6),
};

ENUM_FLAGS_OPERATORS(SymbolType)
ENUM_FLAGS_OPERATORS(SymbolFlags)

class Symbol: public Object
{
    REDASM_OBJECT(Symbol)

    public:
        Symbol();
        Symbol(SymbolType type, SymbolFlags flags, tag_t tag, address_t address, const String& name);
        void lock();
        bool is(SymbolType t) const;
        bool typeIs(SymbolType t) const;
        bool hasFlag(SymbolFlags flag) const;
        bool isFunction() const;
        bool isImport() const;
        bool isExport() const;
        bool isEntryPoint() const;
        bool isLocked() const;
        bool isPointer() const;
        bool isData() const;
        bool isCode() const;
        bool isWeak() const;

    public:
        void save(cereal::BinaryOutputArchive &a) const override;
        void load(cereal::BinaryInputArchive &a) override;

    public:
        SymbolType type;
        SymbolFlags flags;
        tag_t tag;
        address_t address;
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
        size_t size() const;
        Symbol *get(address_t address) const;
        Symbol *get(const String& name) const;
        bool erase(address_t address);
        void clear();

   public:
        bool rename(address_t address, const String& newname);
        void create(address_t address, const String& name, SymbolType type, SymbolFlags flags, tag_t tag = 0);
        void create(address_t address, const String& name, SymbolType type, tag_t tag = 0);
        void create(address_t address, SymbolType type, SymbolFlags flags, tag_t tag = 0);
        void create(address_t address, SymbolType type, tag_t tag = 0);

    public:
        void save(cereal::BinaryOutputArchive &a) const override;
        void load(cereal::BinaryInputArchive &a) override;

    public:
        static String normalized(const String &s);
        static String name(address_t address, SymbolType type, SymbolFlags flags = SymbolFlags::None);
        static String name(address_t address, const String& s, SymbolType type, SymbolFlags flags = SymbolFlags::None);
        static String name(const String& name, address_t address);
};

} // namespace REDasm

VISITABLE_STRUCT(REDasm::Symbol, type, tag, address, name);
