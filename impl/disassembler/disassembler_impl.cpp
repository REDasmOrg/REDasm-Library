#include "disassembler_impl.h"
#include <redasm/disassembler/listing/listingfunctions.h>
#include <redasm/plugins/assembler/assembler.h>
#include <redasm/database/signaturedb.h>
#include <redasm/support/path.h>
#include <redasm/context.h>
#include <cctype>

namespace REDasm {

DisassemblerImpl::DisassemblerImpl(Assembler *assembler, Loader *loader): m_assembler(assembler), m_loader(loader) { }
Loader *DisassemblerImpl::loader() const { return m_loader; }
Assembler *DisassemblerImpl::assembler() const { return m_assembler; }
const safe_ptr<ListingDocumentType> &DisassemblerImpl::document() const { return m_loader->document(); }
safe_ptr<ListingDocumentType> &DisassemblerImpl::document() { return m_loader->document(); }

std::deque<ListingItem*> DisassemblerImpl::getCalls(address_t address)
{

}

ReferenceTable *DisassemblerImpl::references() { return &m_referencetable; }

Printer *DisassemblerImpl::createPrinter()
{

}

ReferenceVector DisassemblerImpl::getReferences(address_t address) const { return m_referencetable.referencesToVector(address); }
ReferenceSet DisassemblerImpl::getTargets(address_t address) const { return m_referencetable.targets(address); }

BufferView DisassemblerImpl::getFunctionBytes(address_t address)
{

}

Symbol *DisassemblerImpl::dereferenceSymbol(const Symbol *symbol, u64 *value)
{
    address_t address = 0;
    Symbol* ptrsymbol = nullptr;

    if(symbol->is(SymbolType::Pointer) && this->dereference(symbol->address, &address))
        ptrsymbol = this->document()->symbol(address);

    if(value)
        *value = address;

    return ptrsymbol;
}

InstructionPtr DisassemblerImpl::disassembleInstruction(address_t address)
{

}

address_location DisassemblerImpl::getTarget(address_t address) const { return m_referencetable.target(address); }
size_t DisassemblerImpl::getTargetsCount(address_t address) const { return m_referencetable.targetsCount(address); }
size_t DisassemblerImpl::getReferencesCount(address_t address) const { return m_referencetable.referencesCount(address); }

size_t DisassemblerImpl::checkAddressTable(const InstructionPtr &instruction, address_t startaddress)
{
    Symbol* symbol = this->document()->symbol(startaddress);

    if(symbol && symbol->is(SymbolType::TableItemMask))
        return REDasm::npos;

    address_t target = 0, address = startaddress;

    if(!this->readAddress(address, m_assembler->addressWidth(), &target))
        return 0;

    r_ctx->statusAddress("Checking address table", startaddress);
    std::unordered_set<address_t> targets;

    while(this->readAddress(address, m_assembler->addressWidth(), &target))
    {
        const Segment* segment = this->document()->segment(target);

        if(!segment || !segment->is(SegmentType::Code))
            break;

        targets.insert(target);

        if(instruction->is(InstructionType::Branch))
            this->pushTarget(target, instruction->address);
        else
            this->checkLocation(startaddress, target);

        address += m_assembler->addressWidth();
    }

    if(!targets.empty())
    {
        if(targets.size() > 1)
        {
            size_t i = 0;
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
            this->document()->pointer(startaddress, SymbolType::Data);
        }
    }

    return targets.size();
}

size_t DisassemblerImpl::locationIsString(address_t address, bool *wide) const
{
    const Segment* segment = this->document()->segment(address);

    if(!segment || segment->is(SegmentType::Bss))
        return 0;

    if(wide) *wide = false;

    size_t count = this->locationIsStringT<u8>(address,
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

size_t DisassemblerImpl::state() const
{

}

std::string DisassemblerImpl::readString(const Symbol *symbol, size_t len) const
{
    address_t memaddress = 0;

    if(symbol->is(SymbolType::Pointer) && this->dereference(symbol->address, &memaddress))
        return this->readString(memaddress, len);

    return this->readString(symbol->address, len);
}

std::string DisassemblerImpl::readString(address_t address, size_t len) const
{
    return this->readStringT<char>(address, len, [](char b, std::string& s) {
        bool r = ::isprint(b) || ::isspace(b);
        if(r) s += b;
        return r;
    });
}

std::string DisassemblerImpl::readWString(const Symbol *symbol, size_t len) const
{
    address_t memaddress = 0;

    if(symbol->is(SymbolType::Pointer) && this->dereference(symbol->address, &memaddress))
        return this->readWString(memaddress, len);

    return this->readWString(symbol->address, len);
}

std::string DisassemblerImpl::readWString(address_t address, size_t len) const
{
    return this->readStringT<u16>(address, len, [](u16 wb, std::string& s) {
        u8 b1 = wb & 0xFF, b2 = (wb & 0xFF00) >> 8;
        bool r = !b2 && (::isprint(b1) || ::isspace(b1));
        if(r) s += static_cast<char>(b1);
        return r;
    });
}

std::string DisassemblerImpl::getHexDump(address_t address, const Symbol **ressymbol)
{

}

bool DisassemblerImpl::loadSignature(const std::string &signame)
{
    std::string signaturefile = Path::exists(signame) ? signame : r_ctx->signature(signame);

    if(!Utils::endsWith(signaturefile, ".json"))
        signaturefile += ".json";

    SignatureDB sigdb;

    if(!sigdb.load(signaturefile))
    {
        r_ctx->log("Failed to load " + Utils::quoted(signaturefile));
        return false;
    }

    if(!sigdb.isCompatible(m_pimpl_q))
    {
        r_ctx->log("Signature " + Utils::quoted(sigdb.name()) + " is not compatible");
        return false;
    }

    r_ctx->log("Loading Signature: " + Utils::quoted(sigdb.name()));
    size_t c = 0;

    this->document()->symbols()->iterate(SymbolType::FunctionMask, [&](const Symbol* symbol) -> bool {
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
        r_ctx->log("Found " + std::to_string(c) + " signature(s)");
    else
        r_ctx->log("No signatures found");

    return true;
}

bool DisassemblerImpl::busy() const
{

}

bool DisassemblerImpl::checkString(address_t fromaddress, address_t address)
{
    bool wide = false;

    if(this->locationIsString(address, &wide) < MIN_STRING)
        return false;

    if(wide)
    {
        this->document()->symbol(address, SymbolType::WideString);
        this->document()->autoComment(fromaddress, "WIDE STRING: " + Utils::quoted(this->readWString(address)));
    }
    else
    {
        this->document()->symbol(address, SymbolType::String);
        this->document()->autoComment(fromaddress, "STRING: " + Utils::quoted(this->readString(address)));
    }

    this->pushReference(address, fromaddress);
    return true;
}

bool DisassemblerImpl::readAddress(address_t address, size_t size, u64 *value) const
{
    if(!value)
        return false;

    const Segment* segment = this->document()->segment(address);

    if(!segment || segment->is(SegmentType::Bss))
        return false;

    offset_location offset = m_loader->offset(address);

    if(!offset.valid)
        return false;

    return this->readOffset(offset, size, value);
}

bool DisassemblerImpl::readOffset(offset_t offset, size_t size, u64 *value) const
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
        r_ctx->problem("Invalid size: " + std::to_string(size));
        return false;
    }

    return true;
}

bool DisassemblerImpl::dereference(address_t address, u64 *value) const
{
    if(!value)
        return false;

    return this->readAddress(address, m_assembler->addressWidth(), value);
}

void DisassemblerImpl::disassemble(address_t address)
{

}

void DisassemblerImpl::popTarget(address_t address, address_t pointedby) { m_referencetable.popTarget(address, pointedby); }
void DisassemblerImpl::pushTarget(address_t address, address_t pointedby) { m_referencetable.pushTarget(address, pointedby); }
void DisassemblerImpl::pushReference(address_t address, address_t refby) { m_referencetable.push(address, refby); }

void DisassemblerImpl::checkLocation(address_t fromaddress, address_t address)
{
    Segment* segment = this->document()->segment(address);

    if(!segment || this->checkString(fromaddress, address))
        return;

    if(!this->document()->symbol(address))
        this->document()->symbol(address, SymbolType::Data);

    this->pushReference(address, fromaddress);
}

void DisassemblerImpl::computeBasicBlocks()
{
    auto lock = x_lock_safe_ptr(m_loader->document());
    ListingFunctions* lf = lock->functions();
    lf->invalidateGraphs();

    for(size_t i = 0; i < lf->size(); i++)
        this->computeBasicBlocks(lock, lf->at(i));
}

void DisassemblerImpl::disassemble()
{

}

void DisassemblerImpl::stop()
{

}

void DisassemblerImpl::pause()
{

}

void DisassemblerImpl::resume()
{

}

void DisassemblerImpl::computeBasicBlocks(document_x_lock &lock, const ListingItem *functionitem)
{

}

} // namespace REDasm
