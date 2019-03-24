#include "disassemblerbase.h"
#include "../database/signaturedb.h"
#include <cctype>

namespace REDasm {

DisassemblerBase::DisassemblerBase(AssemblerPlugin* assembler, LoaderPlugin *loader): DisassemblerAPI(), m_document(loader->document())
{
    m_loader = std::unique_ptr<LoaderPlugin>(loader);
    m_assembler = std::unique_ptr<AssemblerPlugin>(assembler);
}

LoaderPlugin *DisassemblerBase::loader() const { return m_loader.get(); }
AssemblerPlugin *DisassemblerBase::assembler() const { return m_assembler.get(); }
ReferenceVector DisassemblerBase::getReferences(address_t address) { return m_referencetable.referencesToVector(address); }
u64 DisassemblerBase::getReferencesCount(address_t address) { return m_referencetable.referencesCount(address); }
void DisassemblerBase::pushReference(address_t address, address_t refbyaddress) { m_referencetable.push(address, refbyaddress); }

void DisassemblerBase::checkLocation(address_t fromaddress, address_t address)
{
    Segment* segment = m_document->segment(address);

    if(!segment || this->checkString(fromaddress, address))
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

s64 DisassemblerBase::checkAddressTable(const InstructionPtr &instruction, address_t startaddress)
{
    Symbol* symbol = m_document->symbol(startaddress);

    if(symbol && symbol->is(SymbolTypes::TableItemMask))
        return -1;

    address_t target = 0, address = startaddress;

    if(!this->readAddress(address, m_assembler->addressWidth(), &target))
        return 0;

    REDasm::statusAddress("Checking address table", startaddress);
    std::unordered_set<address_t> items;

    while(this->readAddress(address, m_assembler->addressWidth(), &target))
    {
        const Segment* segment = m_document->segment(target);

        if(!segment || !segment->is(SegmentTypes::Code))
            break;

        items.insert(target);

        if(instruction->is(InstructionTypes::Branch))
            instruction->target(target);
        else
            this->checkLocation(startaddress, target);

        address += m_assembler->addressWidth();
    }

    if(!items.empty())
    {
        m_document->update(instruction);

        if(items.size() > 1)
        {
            u64 i = 0;
            address = startaddress;

            for(auto it = items.begin(); it != items.end(); it++, address += m_assembler->addressWidth(), i++)
            {
                if(address == startaddress)
                    m_document->table(address, items.size());
                else
                    m_document->tableItem(address, startaddress, i);

                this->pushReference(address, instruction->address);
            }
        }
        else
        {
            this->pushReference(startaddress, instruction->address);
            m_document->pointer(startaddress, SymbolTypes::Data);
        }
    }

    return items.size();
}

ListingDocument& DisassemblerBase::document() { return m_document; }
ReferenceTable *DisassemblerBase::references() { return &m_referencetable; }

u64 DisassemblerBase::locationIsString(address_t address, bool *wide) const
{
    Segment* segment = m_document->segment(address);

    if(!segment || segment->is(SegmentTypes::Bss))
        return 0;

    if(wide) *wide = false;

    u64 count = this->locationIsStringT<u8>(address,
                                            [](u16 b) -> bool { return ::isprint(b) || ::isspace(b); },
                                            [](u16 b) -> bool {  return (b == '_') || ::isalnum(b) || ::isspace(b); });

    if(count == 1) // Try with wide strings
    {
        count = this->locationIsStringT<u16>(address,
                                             [](u16 wb) -> bool { u8 b1 = wb & 0xFF, b2 = (wb & 0xFF00) >> 8; return !b2 && (::isprint(b1) || ::isspace(b1)); },
                                             [](u16 wb) -> bool { u8 b1 = wb & 0xFF, b2 = (wb & 0xFF00) >> 8; return ( (b1 == '_') || ::isalnum(b1) || ::isspace(b1)) && !b2; });

        if(wide)
            *wide = true;
    }

    return count;
}

std::string DisassemblerBase::readString(const Symbol* symbol, u64 len) const
{
    address_t memaddress = 0;

    if(symbol->is(SymbolTypes::Pointer) && this->dereference(symbol->address, &memaddress))
        return this->readString(memaddress, len);

    return this->readString(symbol->address, len);
}

std::string DisassemblerBase::readWString(const Symbol* symbol, u64 len) const
{
    address_t memaddress = 0;

    if(symbol->is(SymbolTypes::Pointer) && this->dereference(symbol->address, &memaddress))
        return this->readWString(memaddress, len);

    return this->readWString(symbol->address, len);
}

std::string DisassemblerBase::getHexDump(address_t address, const Symbol **ressymbol)
{
    REDasm::ListingItem* item = m_document->functionStart(address);

    if(!item)
        return std::string();

    const REDasm::Symbol* symbol = m_document->symbol(item->address);

    if(!symbol)
        return std::string();

    REDasm::BufferView br = this->getFunctionBytes(symbol->address);

    if(br.eob())
        return std::string();

    if(ressymbol)
        *ressymbol = symbol;

    return REDasm::hexstring(br, br.size());
}

Symbol* DisassemblerBase::dereferenceSymbol(const Symbol *symbol, u64* value)
{
    address_t address = 0;
    Symbol* ptrsymbol = nullptr;

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

    return this->readAddress(address, m_assembler->addressWidth(), value);
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
            const Symbol* symbol = m_document->symbol((*it)->address);

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

    BufferView view = m_loader->view(address);

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

    offset_location offset = m_loader->offset(address);

    if(!offset.valid)
        return false;

    return this->readOffset(offset, size, value);
}

bool DisassemblerBase::readOffset(offset_t offset, size_t size, u64 *value) const
{
    if(!value)
        return false;

    BufferView viewdest = m_loader->viewOffset(offset);

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

std::string DisassemblerBase::readString(address_t address, u64 len) const
{
    return this->readStringT<char>(address, len, [](char b, std::string& s) {
        bool r = ::isprint(b) || ::isspace(b);
        if(r) s += b;
        return r;
    });
}

std::string DisassemblerBase::readWString(address_t address, u64 len) const
{
    return this->readStringT<u16>(address, len, [](u16 wb, std::string& s) {
        u8 b1 = wb & 0xFF, b2 = (wb & 0xFF00) >> 8;
        bool r = !b2 && (::isprint(b1) || ::isspace(b1));
        if(r) s += static_cast<char>(b1);
        return r;
    });
}

bool DisassemblerBase::loadSignature(const std::string &signame)
{
    std::string signaturefile = REDasm::isPath(signame) ? signame : REDasm::makeSignaturePath(signame);

    if(!REDasm::endsWith(signaturefile, ".json"))
        signaturefile += ".json";

    SignatureDB sigdb;

    if(!sigdb.load(signaturefile))
    {
        REDasm::log("Failed to load " + REDasm::quoted(signaturefile));
        return false;
    }

    if(!sigdb.isCompatible(this))
    {
        REDasm::log("Signature " + REDasm::quoted(sigdb.name()) + " is not compatible");
        return false;
    }

    REDasm::log("Loading Signature: " + REDasm::quoted(sigdb.name()));
    u64 c = 0;

    m_document->symbols()->iterate(SymbolTypes::FunctionMask, [&](const Symbol* symbol) -> bool {
        if(symbol->isLocked())
            return true;

        BufferView view = this->getFunctionBytes(symbol->address);
        offset_location offset = m_loader->offset(symbol->address);

        if(view.eob() || !offset.valid)
            return true;

        sigdb.search(view, [&](const json& signature) {
            std::string signame = signature["name"];
            m_document->lock(symbol->address, signame, signature["symboltype"]);
            c++;
        });

        return true;
    });

    if(c)
        REDasm::log("Found " + std::to_string(c) + " signature(s)");
    else
        REDasm::log("No signatures found");

    return true;
}

} // namespace REDasm
