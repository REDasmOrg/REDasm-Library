#include "disassemblerbase.h"
#include "../database/signaturedb.h"
#include "../graph/functiongraph.h"
#include <cctype>

namespace REDasm {

DisassemblerBase::DisassemblerBase(AssemblerPlugin* assembler, LoaderPlugin *loader): DisassemblerAPI()
{
    m_loader = std::unique_ptr<LoaderPlugin>(loader);
    m_assembler = std::unique_ptr<AssemblerPlugin>(assembler);
}

LoaderPlugin *DisassemblerBase::loader() const { return m_loader.get(); }
AssemblerPlugin *DisassemblerBase::assembler() const { return m_assembler.get(); }
const ListingDocument& DisassemblerBase::document() const { return m_loader->document(); }
ListingDocument& DisassemblerBase::document() { return m_loader->document(); }
ReferenceVector DisassemblerBase::getReferences(address_t address) const { return m_referencetable.referencesToVector(address); }
ReferenceSet DisassemblerBase::getTargets(address_t address) const { return m_referencetable.targets(address); }

ListingItems DisassemblerBase::getCalls(address_t address)
{
    auto& document = this->document();
    auto it = document->instructionItem(address);
    ListingItems calls;

    for( ; it != document->end(); it++)
    {
        ListingItem* item = it->get();

        if(item->is(ListingItem::InstructionItem))
        {
            InstructionPtr instruction = document->instruction(item->address);

            if(!instruction->is(InstructionTypes::Call))
                continue;

            calls.push_back(item);
        }
        else if(item->is(ListingItem::SymbolItem))
        {
            const Symbol* symbol = document->symbol(item->address);

            if(!symbol->is(SymbolTypes::Code))
                break;
        }
        else
            break;
    }

    return calls;
}

address_location DisassemblerBase::getTarget(address_t address) const { return m_referencetable.target(address); }
u64 DisassemblerBase::getTargetsCount(address_t address) const { return m_referencetable.targetsCount(address); }
u64 DisassemblerBase::getReferencesCount(address_t address) const { return m_referencetable.referencesCount(address); }

void DisassemblerBase::computeBounds()
{
    REDasm::log("Calculating function bounds...");

    auto lock = x_lock_safe_ptr(m_loader->document());
    lock->functions().invalidateBounds();

    for(ListingItem* item : lock->functions())
        this->computeBounds(lock, item);
}

void DisassemblerBase::popTarget(address_t address, address_t pointedby) { m_referencetable.popTarget(address, pointedby); }
void DisassemblerBase::pushTarget(address_t address, address_t pointedby) { m_referencetable.pushTarget(address, pointedby); }
void DisassemblerBase::pushReference(address_t address, address_t refby) { m_referencetable.push(address, refby); }

void DisassemblerBase::checkLocation(address_t fromaddress, address_t address)
{
    Segment* segment = this->document()->segment(address);

    if(!segment || this->checkString(fromaddress, address))
        return;

    if(!this->document()->symbol(address))
        this->document()->symbol(address, SymbolTypes::Data);

    this->pushReference(address, fromaddress);
}

bool DisassemblerBase::checkString(address_t fromaddress, address_t address)
{
    bool wide = false;

    if(this->locationIsString(address, &wide) < MIN_STRING)
        return false;

    if(wide)
    {
        this->document()->symbol(address, SymbolTypes::WideString);
        this->document()->autoComment(fromaddress, "WIDE STRING: " + REDasm::quoted(this->readWString(address)));
    }
    else
    {
        this->document()->symbol(address, SymbolTypes::String);
        this->document()->autoComment(fromaddress, "STRING: " + REDasm::quoted(this->readString(address)));
    }

    this->pushReference(address, fromaddress);
    return true;
}

s64 DisassemblerBase::checkAddressTable(const InstructionPtr &instruction, address_t startaddress)
{
    Symbol* symbol = this->document()->symbol(startaddress);

    if(symbol && symbol->is(SymbolTypes::TableItemMask))
        return -1;

    address_t target = 0, address = startaddress;

    if(!this->readAddress(address, m_assembler->addressWidth(), &target))
        return 0;

    REDasm::statusAddress("Checking address table", startaddress);
    std::unordered_set<address_t> targets;

    while(this->readAddress(address, m_assembler->addressWidth(), &target))
    {
        const Segment* segment = this->document()->segment(target);

        if(!segment || !segment->is(SegmentTypes::Code))
            break;

        targets.insert(target);

        if(instruction->is(InstructionTypes::Branch))
            this->pushTarget(target, instruction->address);
        else
            this->checkLocation(startaddress, target);

        address += m_assembler->addressWidth();
    }

    if(!targets.empty())
    {
        if(targets.size() > 1)
        {
            u64 i = 0;
            address = startaddress;

            for(auto it = targets.begin(); it != targets.end(); it++, address += m_assembler->addressWidth(), i++)
            {
                if(address == startaddress)
                    this->document()->table(address, targets.size());
                else
                    this->document()->tableItem(address, startaddress, i);

                this->pushReference(address, instruction->address);
            }
        }
        else
        {
            this->pushReference(startaddress, instruction->address);
            this->document()->pointer(startaddress, SymbolTypes::Data);
        }
    }

    return targets.size();
}

ReferenceTable *DisassemblerBase::references() { return &m_referencetable; }

u64 DisassemblerBase::locationIsString(address_t address, bool *wide) const
{
    const Segment* segment = this->document()->segment(address);

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
    REDasm::ListingItem* item = this->document()->functionStart(address);

    if(!item)
        return std::string();

    const REDasm::Symbol* symbol = this->document()->symbol(item->address);

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
        ptrsymbol = this->document()->symbol(address);

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
    ListingItem* item = this->document()->functionStart(address);

    if(!item)
        return BufferView();

    auto it = this->document()->functionItem(item->address);

    if(it == this->document()->end())
        return BufferView();

    it++;
    address_t endaddress = 0;

    for( ; it != this->document()->end(); it++)
    {
        if((*it)->type == ListingItem::SymbolItem)
        {
            const Symbol* symbol = this->document()->symbol((*it)->address);

            if(!symbol->is(SymbolTypes::Code))
                break;

            continue;
        }

        if((*it)->type == ListingItem::InstructionItem)
        {
            InstructionPtr instruction = this->document()->instruction((*it)->address);
            endaddress = instruction->endAddress();
            continue;
        }

        break;
    }

    BufferView view = m_loader->view(address);

    if(it != this->document()->end())
        view.resize(endaddress - address);

    return view;
}

bool DisassemblerBase::readAddress(address_t address, size_t size, u64 *value) const
{
    if(!value)
        return false;

    const Segment* segment = this->document()->segment(address);

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

    this->document()->symbols()->iterate(SymbolTypes::FunctionMask, [&](const Symbol* symbol) -> bool {
        if(symbol->isLocked())
            return true;

        BufferView view = this->getFunctionBytes(symbol->address);
        offset_location offset = m_loader->offset(symbol->address);

        if(view.eob() || !offset.valid)
            return true;

        sigdb.search(view, [&](const json& signature) {
            std::string signame = signature["name"];
            this->document()->lock(symbol->address, signame, signature["symboltype"]);
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

void DisassemblerBase::computeBounds(document_x_lock &lock, ListingItem *functionitem)
{
    Graphing::FunctionGraph fg(this);

    if(!fg.build(functionitem))
    {
        REDasm::log("Cannot compute bounds @ " + REDasm::hex(functionitem->address));
        return;
    }

    for(const Graphing::Node& n : fg.nodes())
    {
        const Graphing::FunctionBasicBlock* fbb = fg.data(n);

        if(fbb)
            lock->functions().bounds(functionitem, { fbb->startidx, fbb->endidx });
        else
            REDasm::log("Incomplete blocks @ " + REDasm::hex(functionitem->address));
    }
}

} // namespace REDasm
