#pragma once

#include <unordered_map>
#include "../../support/serializer.h"
#include "../../support/event.h"
#include "../../redasm.h"

namespace REDasm {

namespace SymbolTypes {
    enum: u32 {
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
}

struct Symbol
{
    Symbol(): type(0), tag(0), address(0), size(0) { }
    Symbol(u32 type, u32 tag, address_t address, const std::string& name): type(type), tag(tag), address(address), size(0), name(name) { }
    void lock() { type |= SymbolTypes::Locked; }

    u32 type, tag;
    address_t address;
    u64 size;
    std::string name;

    constexpr bool is(u32 t) const { return type & t; }
    constexpr bool isFunction() const { return type & SymbolTypes::FunctionMask; }
    constexpr bool isImport() const { return type & SymbolTypes::ImportMask; }
    constexpr bool isLocked() const { return type & SymbolTypes::Locked; }
};

typedef std::unique_ptr<Symbol> SymbolPtr;

class SymbolTable: public Serializer::Serializable
{
    public:
        Event<const Symbol*> deserialized;

    private:
        typedef std::unordered_map<address_t, SymbolPtr> SymbolsByAddress;
        typedef std::unordered_map<std::string, address_t> SymbolsByName;

    public:
        SymbolTable() = default;
        u64 size() const;
        bool create(address_t address, const std::string& name, u32 type, u32 tag = 0);
        Symbol *symbol(address_t address) const;
        Symbol *symbol(const std::string& name) const;
        void iterate(u32 symbolflags, const std::function<bool(const Symbol*)> &cb) const;
        bool erase(address_t address);
        void clear();

    public:
        static std::string name(address_t address, u32 type);
        static std::string name(address_t address, const std::string& s, u32 type);
        virtual void serializeTo(std::fstream& fs);
        virtual void deserializeFrom(std::fstream& fs);

    private:
        static std::string prefix(u32 type);
        void serializeSymbol(std::fstream& fs, const SymbolPtr& value);
        void deserializeSymbol(std::fstream& fs, SymbolPtr& value);
        void bindName(const Symbol *symbol);

    private:
        SymbolsByAddress m_byaddress;
        SymbolsByName m_byname;
};

}
