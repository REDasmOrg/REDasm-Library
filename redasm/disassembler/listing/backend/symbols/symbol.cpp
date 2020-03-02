#include "symbol.h"

namespace REDasm {

Symbol::Symbol(type_t type, flag_t flags, tag_t tag, address_t address, const String& name): type(type), flags(flags), tag(tag), address(address), name(name) { }
bool Symbol::isFunction() const { return REDasm::typeIs(this, Symbol::T_Function); }
bool Symbol::isImport() const { return REDasm::typeIs(this, Symbol::T_Import); }
bool Symbol::isLabel() const { return REDasm::typeIs(this, Symbol::T_Label); }
bool Symbol::isString() const { return REDasm::typeIs(this, Symbol::T_String); }
bool Symbol::isAsciiString() const { return this->isString() && REDasm::hasFlag(this, Symbol::F_AsciiString);  }
bool Symbol::isWideString() const { return this->isString() && REDasm::hasFlag(this, Symbol::F_WideString);  }
bool Symbol::isEntryPoint() const { return REDasm::hasFlag(this, Symbol::F_EntryPoint); }
bool Symbol::isTableItem() const { return REDasm::hasFlag(this, Symbol::F_TableItem); }
bool Symbol::isPointer() const { return REDasm::hasFlag(this, Symbol::F_Pointer); }
bool Symbol::isExport() const { return REDasm::hasFlag(this, Symbol::F_Export); }

bool Symbol::isData() const
{
    switch(this->type)
    {
        case Symbol::T_Data:
        case Symbol::T_String: return true;
        default: break;
    }

    return false;
}

bool Symbol::isCode() const
{
    switch(this->type)
    {
        case Symbol::T_Label:
        case Symbol::T_Function: return true;
        default: break;
    }

    return false;
}

bool Symbol::isWeak() const { return REDasm::hasFlag(this, Symbol::F_Weak); }

} // namespace REDasm
