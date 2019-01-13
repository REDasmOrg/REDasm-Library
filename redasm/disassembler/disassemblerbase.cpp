#include "disassemblerbase.h"
#include "../database/signaturedb.h"
#include <cctype>

namespace REDasm {

DisassemblerBase::DisassemblerBase(FormatPlugin *format): DisassemblerAPI(), m_document(format->document()) { m_format = std::unique_ptr<FormatPlugin>(format); }
DisassemblerBase::~DisassemblerBase() { }
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
    bool wide = false;

    if(this->locationIsString(address, &wide) < MIN_STRING)
        return false;

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
    address_t target = 0, address = startaddress;

    if(!this->readAddress(address, m_format->addressWidth(), &target))
        return 0;

    REDasm::status("Checking address table @ " + REDasm::hex(startaddress, m_format->bits()));
    int c = 0;

    while(this->readAddress(address, m_format->addressWidth(), &target))
    {
        const Segment* segment = m_document->segment(target);

        if(!segment || !segment->is(SegmentTypes::Code))
            break;

        instruction->target(target);
        address += m_format->addressWidth();
        c++;
    }

    if(c)
    {
        this->pushReference(startaddress, instruction->address);
        m_document->update(instruction);

        if(c > 1)
            m_document->table(startaddress, c);
        else
            m_document->pointer(startaddress, SymbolTypes::Data);
    }

    return c;
}

FormatPlugin *DisassemblerBase::format() { return m_format.get(); }
ListingDocument& DisassemblerBase::document() { return m_document; }
ReferenceTable *DisassemblerBase::references() { return &m_referencetable; }

u64 DisassemblerBase::locationIsString(address_t address, bool *wide) const
{
    Segment* segment = m_document->segment(address);

    if(!segment || segment->is(SegmentTypes::Bss))
        return 0;

    if(wide)
        *wide = false;

    u64 count = this->locationIsStringT<u8>(address, ::isprint, [](u16 b) -> bool { return ::isalnum(b) || ::isspace(b); });

    if(count == 1) // Try with wide strings
    {
        count = this->locationIsStringT<u16>(address, [](u16 wb) -> bool { u8 b1 = wb & 0xFF, b2 = (wb & 0xFF00) >> 8; return ::isprint(b1) && !b2; },
                                                      [](u16 wb) -> bool { u8 b1 = wb & 0xFF, b2 = (wb & 0xFF00) >> 8; return (::isspace(b1) || ::isalnum(b1)) && !b2; } );

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

BufferRef DisassemblerBase::getFunctionBytes(address_t address)
{
    ListingItem* item = m_document->functionStart(address);

    if(!item)
        return BufferRef();

    auto it = m_document->functionItem(item->address);

    if(it == m_document->end())
        return BufferRef();

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

    BufferRef br = m_format->buffer(address);

    if(it != m_document->end())
        br.resize(endaddress - address);

    return br;
}

bool DisassemblerBase::readAddress(address_t address, size_t size, u64 *value) const
{
    if(!value)
        return false;

    Segment* segment = m_document->segment(address);

    if(!segment || segment->is(SegmentTypes::Bss))
        return false;

    return this->readOffset(m_format->offset(address), size, value);
}

bool DisassemblerBase::readOffset(offset_t offset, size_t size, u64 *value) const
{
    if(!value)
        return false;

    BufferRef pdest = m_format->buffer().slice(offset);

    if(size == 1)
        *value = static_cast<u8>(pdest);
    else if(size == 2)
        *value = static_cast<u16>(pdest);
    else if(size == 4)
        *value = static_cast<u32>(pdest);
    else if(size == 8)
        *value = static_cast<u64>(pdest);
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

        BufferRef br = this->getFunctionBytes(symbol->address);

        if(br.empty())
            return true;

        offset_t offset = m_format->offset(symbol->address);

        sigdb.search(br, [&](const Signature* signature) {
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
