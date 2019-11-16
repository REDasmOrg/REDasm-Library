#include "disassembler_impl.h"
#include "../libs/cereal/archives/binary.hpp"
#include "../database/database_impl.h"
#include "engine/stringfinder.h"
#include <redasm/disassembler/listing/backend/listingfunctions.h>
#include <redasm/disassembler/model/functiongraph.h>
#include <redasm/plugins/assembler/assembler.h>
#include <redasm/database/signaturedb.h>
#include <redasm/buffer/memorybuffer.h>
#include <redasm/support/utils.h>
#include <redasm/support/path.h>
#include <redasm/context.h>
#include <cctype>

namespace REDasm {

DisassemblerImpl::DisassemblerImpl(Disassembler* q, Assembler *assembler, Loader *loader): m_pimpl_q(q), m_assembler(assembler), m_loader(loader) { r_ctx->setDisassembler(q); }
DisassemblerImpl::~DisassemblerImpl() { if(m_loader) m_loader->buffer()->release(); }
Loader *DisassemblerImpl::loader() const { return m_loader; }
Assembler *DisassemblerImpl::assembler() const { return m_assembler; }
const safe_ptr<ListingDocumentType>& DisassemblerImpl::documentNew() const { return m_loader->documentNew(); }
safe_ptr<ListingDocumentType>& DisassemblerImpl::documentNew() { return m_loader->documentNew(); }
ReferenceTable *DisassemblerImpl::references() { return &m_referencetable; }
SortedSet DisassemblerImpl::getReferences(address_t address) const { return m_referencetable.references(address); }
SortedSet DisassemblerImpl::getTargets(address_t address) const { return m_referencetable.targets(address); }

BufferView DisassemblerImpl::getFunctionBytes(address_t address)
{
    REDasm::ListingItem item = r_doc->functionStart(address);
    if(!item.isValid()) return BufferView();

    const auto* graph = r_doc->graph(item.address);
    if(!graph) return BufferView();

    ListingItem startitem, enditem;

    graph->nodes().each([&](Node n) {
        const FunctionBasicBlock* fbb = variant_object<FunctionBasicBlock>(graph->data(n));
        if(!fbb) return;

        if(!startitem.isValid()) startitem = fbb->startItem();
        else if(startitem.address > fbb->startItem().address) startitem = fbb->startItem();

        if(!enditem.isValid()) enditem = fbb->endItem();
        else if(enditem.address < fbb->endItem().address) enditem = fbb->endItem();
    });

    if(!startitem.isValid() || !enditem.isValid()) return BufferView();

    BufferView v = this->loader()->view(startitem.address);
    v.resize(enditem.address - startitem.address);
    return v;
}

const Symbol *DisassemblerImpl::dereferenceSymbol(const Symbol *symbol, u64 *value)
{
    address_t address = 0;
    const Symbol* ptrsymbol = nullptr;

    if(symbol->is(SymbolType::Pointer) && this->dereference(symbol->address, &address))
        ptrsymbol = r_doc->symbol(address);

    if(value) *value = address;
    return ptrsymbol;
}

CachedInstruction DisassemblerImpl::decodeInstruction(address_t address)
{
    if(!m_engine) return CachedInstruction();
    return m_engine->decodeInstruction(address);
}

address_location DisassemblerImpl::getTarget(address_t address) const { return m_referencetable.target(address); }
size_t DisassemblerImpl::getTargetsCount(address_t address) const { return m_referencetable.targetsCount(address); }
size_t DisassemblerImpl::getReferencesCount(address_t address) const { return m_referencetable.referencesCount(address); }

size_t DisassemblerImpl::checkAddressTable(const CachedInstruction& instruction, address_t startaddress)
{
    const Symbol* symbol = r_doc->symbol(startaddress);
    if(symbol && symbol->hasFlag(SymbolFlags::TableItem)) return REDasm::npos;

    address_t target = 0, address = startaddress;

    if(!this->readAddress(address, r_asm->addressWidth(), &target))
        return 0;

    r_ctx->statusAddress("Checking address table", startaddress);
    std::unordered_set<address_t> targets;

    while(this->readAddress(address, r_asm->addressWidth(), &target))
    {
        const Segment* segment = r_doc->segment(target);
        if(!segment || !segment->is(SegmentType::Code)) break;

        targets.insert(target);

        if(instruction->typeIs(InstructionType::Branch)) this->pushTarget(target, instruction->address);
        else this->checkLocation(startaddress, target);

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
                if(address == startaddress) r_doc->table(address, targets.size());
                else r_doc->tableItem(address, startaddress, i);
                this->pushReference(address, instruction->address);
            }
        }
        else
        {
            this->pushReference(startaddress, instruction->address);
            r_doc->pointer(startaddress, SymbolType::Data);
        }
    }

    return targets.size();
}

String DisassemblerImpl::readString(const Symbol *symbol, size_t len) const
{
    address_t memaddress = 0;

    if(symbol->is(SymbolType::Pointer) && this->dereference(symbol->address, &memaddress))
        return this->readString(memaddress, len);

    return this->readString(symbol->address, len);
}

String DisassemblerImpl::readString(address_t address, size_t len) const { return this->readStringT<char>(address, len); }
String DisassemblerImpl::readWString(address_t address, size_t len) const { return this->readStringT<char16_t>(address, len); }

String DisassemblerImpl::readWString(const Symbol *symbol, size_t len) const
{
    address_t memaddress = 0;

    if(symbol->is(SymbolType::Pointer) && this->dereference(symbol->address, &memaddress))
        return this->readWString(memaddress, len);

    return this->readWString(symbol->address, len);
}

String DisassemblerImpl::getHexDump(address_t address, const Symbol **ressymbol)
{
    REDasm::ListingItem item = r_doc->functionStart(address);
    if(!item.isValid()) return String();

    const REDasm::Symbol* symbol = r_doc->symbol(item.address);
    if(!symbol) return String();

    REDasm::BufferView br = this->getFunctionBytes(symbol->address);
    if(br.eob()) return String();

    if(ressymbol) *ressymbol = symbol;
    return String::hexstring(br.data(), br.size());
}

bool DisassemblerImpl::loadSignature(const String &signame)
{
    String signaturefile = Path::exists(signame) ? signame : r_ctx->signaturedb(signame);

    if(!signaturefile.endsWith(".json"))
        signaturefile += ".json";

    SignatureDB sigdb;

    if(!sigdb.load(signaturefile))
    {
        r_ctx->log("Failed to load " + signaturefile.quoted());
        return false;
    }

    if(!sigdb.isCompatible())
    {
        r_ctx->log("Signature " + sigdb.name().quoted() + " is not compatible");
        return false;
    }

    r_ctx->log("Loading Signature: " + sigdb.name().quoted());
    size_t c = 0;

    //FIXME: this->document()->symbols()->iterate(SymbolType::FunctionMask, [&](const Symbol* symbol) -> bool {
    //     if(symbol->isLocked())
    //         return true;

    //     BufferView view = this->getFunctionBytes(symbol->address);
    //     offset_location offset = m_loader->offset(symbol->address);

    //     if(view.eob() || !offset.valid)
    //         return true;

    //     sigdb.search(view, [&](const json& signature) {
    //         String signame = signature["name"];
    //         this->document()->lock(symbol->address, signame, signature["symboltype"]);
    //         c++;
    //     });

    //     return true;
    // });

    if(c) r_ctx->log("Found " + String::number(c) + " signature(s)");
    else r_ctx->log("No signatures found");

    return true;
}

bool DisassemblerImpl::busy() const { return m_engine ? m_engine->busy() : false; }
bool DisassemblerImpl::needsWeak() const { return m_engine ? m_engine->needsWeak() : false; }

bool DisassemblerImpl::readAddress(address_t address, size_t size, u64 *value) const
{
    if(!value) return false;

    const Segment* segment = r_doc->segment(address);
    if(!segment || segment->is(SegmentType::Bss)) return false;

    offset_location offset = m_loader->offset(address);
    if(!offset.valid) return false;

    return this->readOffset(offset, size, value);
}

bool DisassemblerImpl::readOffset(offset_t offset, size_t size, u64 *value) const
{
    if(!value) return false;

    BufferView viewdest = r_ldr->viewOffset(offset);

    if(size == 1) *value = static_cast<u8>(viewdest);
    else if(size == 2) *value = static_cast<u16>(viewdest);
    else if(size == 4) *value = static_cast<u32>(viewdest);
    else if(size == 8) *value = static_cast<u64>(viewdest);
    else
    {
        r_ctx->problem("Invalid size: " + String::number(size));
        return false;
    }

    return true;
}

bool DisassemblerImpl::dereference(address_t address, u64 *value) const
{
    if(!value) return false;
    return this->readAddress(address, m_assembler->addressWidth(), value);
}

void DisassemblerImpl::disassemble(address_t address)
{
    m_engine->enqueue(address);
    if(!m_engine->busy()) m_engine->execute(DisassemblerEngineSteps::Algorithm);
}

void DisassemblerImpl::popTarget(address_t address, address_t pointedby) { m_referencetable.popTarget(address, pointedby); }
void DisassemblerImpl::pushTarget(address_t address, address_t pointedby) { m_referencetable.pushTarget(address, pointedby); }
void DisassemblerImpl::pushReference(address_t address, address_t refby) { m_referencetable.push(address, refby); }

void DisassemblerImpl::checkLocation(address_t fromaddress, address_t address)
{
    Segment* segment = r_doc->segment(address);
    if(!segment) return;

    const Symbol* symbol = r_doc->symbol(address);

    if(symbol && symbol->is(SymbolType::StringNew))
    {
        if(symbol->hasFlag(SymbolFlags::WideString))
            r_doc->autoComment(fromaddress, "WIDE STRING: " + this->readWString(address).quoted());
        else
            r_doc->autoComment(fromaddress, "STRING: " + this->readString(address).quoted());
    }
    else
        r_doc->data(address, r_asm->addressWidth());

    this->pushReference(address, fromaddress);
}

void DisassemblerImpl::disassemble()
{
    m_engine = std::make_unique<DisassemblerEngine>();
    if(!r_doc->segmentsCount()) return;

    // Preload functions for analysis
    for(size_t i = 0; i < r_doc->functionsCount(); i++)
        m_engine->enqueue(r_doc->functionAt(i));

    if(m_engine->concurrency() == 1) r_ctx->log("Single threaded disassembly");
    else r_ctx->log("Disassembling with " + String::number(m_engine->concurrency()) + " threads");

    m_engine->execute();
}

void DisassemblerImpl::stop() { if(m_engine) m_engine->stop(); }

template<typename T> String DisassemblerImpl::readStringT(address_t address, size_t len) const
{
    BufferView view = m_loader->view(address);
    size_t i = 0;
    String s;
    char ch;

    for( ; (i < len) && !view.eob(); i++)
    {
        if(!StringFinder::toAscii(static_cast<T>(view), &ch)) break;
        s += ch;
        view += sizeof(T);
    }

    String res = s.simplified();

    if(i > len) res += "...";
    return res;
}

template String DisassemblerImpl::readStringT<char>(address_t, size_t) const;
template String DisassemblerImpl::readStringT<char16_t>(address_t, size_t) const;

} // namespace REDasm
