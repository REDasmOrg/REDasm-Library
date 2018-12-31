#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <unordered_map>
#include <mutex>
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

        Table              = 0x00010000 | Pointer | Data,

        LockedMask         = ~Locked,
        FunctionMask       = Function                      & ~(Code      | Locked),
        ExportMask         = (ExportData | ExportFunction) & ~(Function  | Data | Locked),
        ImportMask         = Import                        & ~(Data      | Locked),
        EntryPointMask     = EntryPoint                    & ~(Function),
        StringMask         = String                        & ~(Pointer),
        WideStringMask     = WideString                    & ~(String    | Pointer),
        TableMask          = Table                         & ~(Pointer   | Data),
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

    bool is(u32 t) const { return type & t; }
    bool isFunction() const { return type & SymbolTypes::FunctionMask; }
    bool isTable() const { return type & SymbolTypes::TableMask; }
    bool isImport() const { return type & SymbolTypes::ImportMask; }
    bool isLocked() const { return type & SymbolTypes::Locked; }
};

typedef std::shared_ptr<Symbol> SymbolPtr;

class SymbolTable: public Serializer::Serializable
{
    public:
        Event<const SymbolPtr&> deserialized;

    private:
        using symbol_lock = std::unique_lock<std::mutex>;
        typedef std::unordered_map<address_t, SymbolPtr> SymbolsByAddress;
        typedef std::unordered_map<std::string, address_t> SymbolsByName;

    public:
        SymbolTable();
        u64 size() const;
        bool create(address_t address, const std::string& name, u32 type, u32 tag = 0);
        SymbolPtr symbol(address_t address);
        SymbolPtr symbol(const std::string& name);
        void iterate(u32 symbolflags, std::function<bool(const SymbolPtr &)> f);
        bool erase(address_t address);

    public:
        virtual void serializeTo(std::fstream& fs);
        virtual void deserializeFrom(std::fstream& fs);

    private:
        void serializeSymbol(std::fstream& fs, const SymbolPtr& value);
        void deserializeSymbol(std::fstream& fs, SymbolPtr& value);
        void bindName(const SymbolPtr& symbol);

    private:
        SymbolsByAddress m_byaddress;
        SymbolsByName m_byname;
        std::mutex m_mutex;
};

}

#endif // SYMBOLTABLE_H
