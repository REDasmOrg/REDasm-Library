#include "disassemblerbase.h"
#include "../database/signaturedb.h"
#include <cctype>

namespace REDasm {

DisassemblerBase::DisassemblerBase(FormatPlugin *format): DisassemblerAPI(), m_document(format->document()) { m_format = std::unique_ptr<FormatPlugin>(format); }
ReferenceVector DisassemblerBase::getReferences(address_t address) { return m_referencetable.referencesToVector(address); }
u64 DisassemblerBase::getReferencesCount(address_t address) { return m_referencetable.referencesCount(address); }
void DisassemblerBase::pushReference(address_t address, address_t refbyaddress) { m_referencetable.push(address, refbyaddress); }

void DisassemblerBase::checkLocation(address_t fromaddress, address_t address)
{
    if(this->checkString(fromaddress, address))
        return;

    Segment* segment = m_document->segment(address);

    if(!segment || (!segment->is(SegmentTypes::Data) && !segment->is(SegmentTypes::Bss))) // Don't flood "Pure-Code" sections with symbols
        return;

    if(!m_document->symbol(address))
        m_document->symbol(address, SymbolTypes::Data);

    this->pushReference(address, fromaddress);
}

bool DisassemblerBase::checkString(address_t fromaddress, address_t address)
{
    bool wide = false, middle = false;

    if(this->locationIsString(address, &wide, &middle) < MIN_STRING)
        return false;

    if(middle) //Ok...it's a string, but we are in the middle of it
        return true;

    if(wide)
    {
        m_document->symbol(address, SymbolTypes::WideString);
        m_document->autoComment(fromaddress, "WIDE STRING: " + REDasm::quoted(this->readWString(address)));
    }
    else
    {
        m_document->symbol(address, SymbolTypes::String);
        m_document->autoComment(fromaddress, "STRING: " + REDasm::quoted(this->readString(address)));
    }

    this->pushReference(address, fromaddress);
    return true;
}

int DisassemblerBase::checkAddressTable(const InstructionPtr &instruction, address_t startaddress)
{
    SymbolPtr symbol = m_document->symbol(startaddress);

    if(symbol && (symbol->isTable() || symbol->is(SymbolTypes::TableItem)))
        return -1;

    address_t target = 0, address = startaddress;

    if(!this->readAddress(address, m_format->addressWidth(), &target))
        return 0;

    REDasm::statusAddress("Checking address table", startaddress);
    std::unordered_set<address_t> items;

    while(this->readAddress(address, m_format->addressWidth(), &target))
    {
        const Segment* segment = m_document->segment(target);

        if(!segment || !segment->is(SegmentTypes::Code))
            break;

        items.insert(target);
        instruction->target(target);
        address += m_format->addressWidth();
    }

    if(!items.empty())
    {
        this->pushReference(startaddress, instruction->address);
        m_document->update(instruction);

        if(items.size() > 1)
        {
            u32 i = 0;

            for(auto it = items.begin(); it != items.end(); it++)
                m_document->tableItem(*it, SymbolTypes::Code, i);

            m_document->table(startaddress, items.size());
        }
        else
            m_document->pointer(startaddress, SymbolTypes::Data);
    }

    return items.size();
}

FormatPlugin *DisassemblerBase::format() { return m_format.get(); }
ListingDocument& DisassemblerBase::document() { return m_document; }
ReferenceTable *DisassemblerBase::references() { return &m_referencetable; }

u64 DisassemblerBase::locationIsString(address_t address, bool *wide, bool* middle) const
{
    Segment* segment = m_document->segment(address);

    if(!segment || segment->is(SegmentTypes::Bss))
        return 0;

    if(wide) *wide = false;
    if(middle) *middle = false;

    u64 count = this->locationIsStringT<u8>(address,
                                            ::isprint, [](u16 b) -> bool {  return (b == '_') || ::isalnum(b) || ::isspace(b); },
                                            middle);

    if(count == 1) // Try with wide strings
    {
        count = this->locationIsStringT<u16>(address,
                                             [](u16 wb) -> bool { u8 b1 = wb & 0xFF, b2 = (wb & 0xFF00) >> 8; return ::isprint(b1) && !b2; },
                                             [](u16 wb) -> bool { u8 b1 = wb & 0xFF, b2 = (wb & 0xFF00) >> 8; return ( (b1 == '_') || ::isalnum(b1) || ::isspace(b1)) && !b2; },
                                             middle);

        if(wide)
            *wide = true;
    }

    return count;
}

std::string DisassemblerBase::readString(const SymbolPtr &symbol) const
{
    address_t memaddress = 0;

    if(symbol->is(SymbolTypes::Pointer) && this->dereference(symbol->address, &memaddress))
        return this->readString(memaddress);

    return this->readString(symbol->address);
}

std::string DisassemblerBase::readWString(const SymbolPtr &symbol) const
{
    address_t memaddress = 0;

    if(symbol->is(SymbolTypes::Pointer) && this->dereference(symbol->address, &memaddress))
        return this->readWString(memaddress);

    return this->readWString(symbol->address);
}

SymbolPtr DisassemblerBase::dereferenceSymbol(const SymbolPtr& symbol, u64* value)
{
    address_t address = 0;
    SymbolPtr ptrsymbol;

    if(symbol->is(SymbolTypes::Pointer) && this->dereference(symbol->address, &address))
        ptrsymbol = m_document->symbol(address);

    if(value)
        *value = address;

    return ptrsymbol;
}

bool DisassemblerBase::dereference(address_t address, u64 *value) const
{
    if(!value)
        return false;

    return this->readAddress(address, m_format->addressWidth(), value);
}

BufferView DisassemblerBase::getFunctionBytes(address_t address)
{
    ListingItem* item = m_document->functionStart(address);

    if(!item)
        return BufferView();

    auto it = m_document->functionItem(item->address);

    if(it == m_document->end())
        return BufferView();

    it++;
    address_t endaddress = 0;

    for( ; it != m_document->end(); it++)
    {
        if((*it)->type == ListingItem::SymbolItem)
        {
            SymbolPtr symbol = m_document->symbol((*it)->address);

            if(!symbol->is(SymbolTypes::Code))
                break;

            continue;
        }

        if((*it)->type == ListingItem::InstructionItem)
        {
            InstructionPtr instruction = m_document->instruction((*it)->address);
            endaddress = instruction->endAddress();
            continue;
        }

        break;
    }

    BufferView view = m_format->view(address);

    if(it != m_document->end())
        view.resize(endaddress - address);

    return view;
}

bool DisassemblerBase::readAddress(address_t address, size_t size, u64 *value) const
{
    if(!value)
        return false;

    Segment* segment = m_document->segment(address);

    if(!segment || segment->is(SegmentTypes::Bss))
        return false;

    offset_location offset = m_format->offset(address);

    if(!offset.valid)
        return false;

    return this->readOffset(offset, size, value);
}

bool DisassemblerBase::readOffset(offset_t offset, size_t size, u64 *value) const
{
    if(!value)
        return false;

    BufferView viewdest = m_format->viewOffset(offset);

    if(size == 1)
        *value = static_cast<u8>(viewdest);
    else if(size == 2)
        *value = static_cast<u16>(viewdest);
    else if(size == 4)
        *value = static_cast<u32>(viewdest);
    else if(size == 8)
        *value = static_cast<u64>(viewdest);
    else
    {
        REDasm::log("Invalid size: " + std::to_string(size));
        return false;
    }

    return true;
}

std::string DisassemblerBase::readString(address_t address) const
{
    return this->readStringT<char>(address, [](char b, std::string& s) {
        bool r = ::isprint(b);
        if(r) s += b;
        return r;
    });
}

std::string DisassemblerBase::readWString(address_t address) const
{
    return this->readStringT<u16>(address, [](u16 wb, std::string& s) {
        u8 b1 = wb & 0xFF, b2 = (wb & 0xFF00) >> 8;
        bool r = ::isprint(b1) && !b2;
        if(r) s += static_cast<char>(b1);
        return r;
    });
}

bool DisassemblerBase::loadSignature(const std::string &sdbfile)
{
    SignatureDB sigdb;

    if(!sigdb.load(sdbfile))
        return false;

    REDasm::log("Loading Signature: " + REDasm::quoted(sdbfile));
    bool found = true;

    m_document->symbols()->iterate(SymbolTypes::FunctionMask, [&](const SymbolPtr& symbol) -> bool {
        if(symbol->isLocked())
            return true;

        BufferView view = this->getFunctionBytes(symbol->address);
        offset_location offset = m_format->offset(symbol->address);

        if(view.eob() || !offset.valid)
            return true;

        sigdb.search(view, [&](const Signature* signature) {
            if(!signature->isCompatible(m_format.get()))
                return;

            REDasm::log("Found " + REDasm::quoted(signature->name) + " @ " + REDasm::hex(symbol->address));

            m_document->lock(symbol->address, signature->name, signature->symboltype);
            found = true;
        });

        return true;
    });

    if(!found)
        REDasm::log("No Signatures Found");

    return true;
}

} // namespace REDasm
