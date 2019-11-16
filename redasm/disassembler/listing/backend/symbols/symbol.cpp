#include "symbol.h"

namespace REDasm {

Symbol::Symbol(SymbolType type, SymbolFlags flags, tag_t tag, address_t address, const String& name): type(type), flags(flags), tag(tag), address(address), name(name) { }
bool Symbol::isFunction() const { return REDasm::typeIs(this, SymbolType::Function); }
bool Symbol::isImport() const { return REDasm::typeIs(this, SymbolType::Import); }
bool Symbol::isLabel() const { return REDasm::typeIs(this, SymbolType::Label); }
bool Symbol::isString() const { return REDasm::typeIs(this, SymbolType::String); }
bool Symbol::isAsciiString() const { return this->isString() && REDasm::hasFlag(this, SymbolFlags::AsciiString);  }
bool Symbol::isWideString() const { return this->isString() && REDasm::hasFlag(this, SymbolFlags::WideString);  }
bool Symbol::isEntryPoint() const { return REDasm::hasFlag(this, SymbolFlags::EntryPoint); }
bool Symbol::isTableItem() const { return REDasm::hasFlag(this, SymbolFlags::TableItem); }
bool Symbol::isPointer() const { return REDasm::hasFlag(this, SymbolFlags::Pointer); }
bool Symbol::isExport() const { return REDasm::hasFlag(this, SymbolFlags::Export); }

bool Symbol::isData() const
{
    switch(this->type)
    {
        case SymbolType::Data:
        case SymbolType::String: return true;
        default: break;
    }

    return false;
}

bool Symbol::isCode() const
{
    switch(this->type)
    {
        case SymbolType::Label:
        case SymbolType::Function: return true;
        default: break;
    }

    return false;
}

bool Symbol::isWeak() const { return REDasm::hasFlag(this, SymbolFlags::Weak); }

} // namespace REDasm
