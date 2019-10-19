#include "disassembler_impl.h"
#include "../libs/cereal/archives/binary.hpp"
#include "../database/database_impl.h"
#include <redasm/disassembler/listing/backend/listingfunctions.h>
#include <redasm/plugins/assembler/assembler.h>
#include <redasm/database/signaturedb.h>
#include <redasm/graph/functiongraph.h>
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
const safe_ptr<ListingDocumentType> &DisassemblerImpl::document() const { return m_loader->document(); }
safe_ptr<ListingDocumentType> &DisassemblerImpl::document() { return m_loader->document(); }
const safe_ptr<ListingDocumentTypeNew>& DisassemblerImpl::documentNew() const { return m_loader->documentNew(); }
safe_ptr<ListingDocumentTypeNew>& DisassemblerImpl::documentNew() { return m_loader->documentNew(); }

SortedList DisassemblerImpl::getCalls(address_t address)
{
    REDasm::ListingItem* item = this->document()->functionStart(address);

    if(!item)
        return SortedList();

    const auto* graph = this->document()->functions()->graph(item->address_new);

    if(!graph)
        return SortedList();

    SortedList calls;

    graph->nodes().each([&](const Variant& v) {
        Node n = v.toInt();

        const FunctionBasicBlock* fbb = variant_object<FunctionBasicBlock>(graph->data(n));

        if(!fbb)
            return;

        for(ListingItem* item = fbb->startItem(); item; item = r_doc->next(item))
        {
            if(!item->is(ListingItemType::InstructionItem))
                continue;

            CachedInstruction instruction = this->document()->instruction(item->address_new);

            if(instruction->is(InstructionType::Call))
                calls.insert(item);

            if(item == fbb->endItem())
                break;
        }
    });

    return calls;
}

ReferenceTable *DisassemblerImpl::references() { return &m_referencetable; }
SortedSet DisassemblerImpl::getReferences(address_t address) const { return m_referencetable.references(address); }
SortedSet DisassemblerImpl::getTargets(address_t address) const { return m_referencetable.targets(address); }

BufferView DisassemblerImpl::getFunctionBytes(address_t address)
{
    REDasm::ListingItem* item = this->document()->functionStart(address);

    if(!item)
        return BufferView();

    const auto* graph = this->document()->functions()->graph(item->address_new);

    if(!graph)
        return BufferView();

    const ListingItem *startitem = nullptr, *enditem = nullptr;

    graph->nodes().each([&](Node n) {
        const FunctionBasicBlock* fbb = variant_object<FunctionBasicBlock>(graph->data(n));

        if(!fbb)
            return;

        if(!startitem)
            startitem = fbb->startItem();
        else if(startitem->address_new > fbb->startItem()->address_new)
            startitem = fbb->startItem();

        if(!enditem)
            enditem = fbb->endItem();
        else if(enditem->address_new < fbb->endItem()->address_new)
            enditem = fbb->endItem();
    });

    if(!startitem || !enditem)
        return BufferView();

    BufferView v = this->loader()->view(startitem->address_new);
    v.resize(enditem->address_new - startitem->address_new);
    return v;
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

CachedInstruction DisassemblerImpl::disassembleInstruction(address_t address)
{
    CachedInstruction instruction = this->document()->instruction(address);

    if(instruction)
        return instruction;

    instruction = this->document()->cacheInstruction(address);
    m_algorithm->disassembleInstruction(address, instruction);
    m_algorithm->done(address);
    return instruction;
}

address_location DisassemblerImpl::getTarget(address_t address) const { return m_referencetable.target(address); }
size_t DisassemblerImpl::getTargetsCount(address_t address) const { return m_referencetable.targetsCount(address); }
size_t DisassemblerImpl::getReferencesCount(address_t address) const { return m_referencetable.referencesCount(address); }

size_t DisassemblerImpl::checkAddressTable(const CachedInstruction& instruction, address_t startaddress)
{
    const Symbol* symbol = r_docnew->symbol(startaddress);
    if(symbol && symbol->hasFlag(SymbolFlags::TableItem)) return REDasm::npos;

    address_t target = 0, address = startaddress;

    if(!this->readAddress(address, r_asm->addressWidth(), &target))
        return 0;

    r_ctx->statusAddress("Checking address table", startaddress);
    std::unordered_set<address_t> targets;

    while(this->readAddress(address, r_asm->addressWidth(), &target))
    {
        const Segment* segment = r_docnew->segment(target);

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
                if(address == startaddress) r_docnew->table(address, targets.size());
                else r_docnew->tableItem(address, startaddress, i);
                this->pushReference(address, instruction->address);
            }
        }
        else
        {
            this->pushReference(startaddress, instruction->address);
            r_docnew->pointer(startaddress, SymbolType::Data);
        }
    }

    return targets.size();
}

size_t DisassemblerImpl::locationIsString(address_t address, bool *wide) const
{
    const Segment* segment = this->documentNew()->segment(address);

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

JobState DisassemblerImpl::state() const { return m_engine ? m_engine->state() : JobState::InactiveState; }

String DisassemblerImpl::readString(const Symbol *symbol, size_t len) const
{
    address_t memaddress = 0;

    if(symbol->is(SymbolType::Pointer) && this->dereference(symbol->address, &memaddress))
        return this->readString(memaddress, len);

    return this->readString(symbol->address, len);
}

String DisassemblerImpl::readString(address_t address, size_t len) const
{
    return this->readStringT<char>(address, len, [](char b, String& s) {
        bool r = ::isprint(b) || ::isspace(b);
        if(r) s += b;
        return r;
    });
}

String DisassemblerImpl::readWString(const Symbol *symbol, size_t len) const
{
    address_t memaddress = 0;

    if(symbol->is(SymbolType::Pointer) && this->dereference(symbol->address, &memaddress))
        return this->readWString(memaddress, len);

    return this->readWString(symbol->address, len);
}

String DisassemblerImpl::readWString(address_t address, size_t len) const
{
    return this->readStringT<u16>(address, len, [](u16 wb, String& s) {
        u8 b1 = wb & 0xFF, b2 = (wb & 0xFF00) >> 8;
        bool r = !b2 && (::isprint(b1) || ::isspace(b1));
        if(r) s += static_cast<char>(b1);
        return r;
    });
}

String DisassemblerImpl::getHexDump(address_t address, const Symbol **ressymbol)
{
    const REDasm::ListingItem* item = this->document()->functionStart(address);

    if(!item)
        return String();

    const REDasm::Symbol* symbol = this->document()->symbol(item->address_new);

    if(!symbol)
        return String();

    REDasm::BufferView br = this->getFunctionBytes(symbol->address);

    if(br.eob())
        return String();

    if(ressymbol)
        *ressymbol = symbol;

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

    this->document()->symbols()->iterate(SymbolType::FunctionMask, [&](const Symbol* symbol) -> bool {
        if(symbol->isLocked())
            return true;

        BufferView view = this->getFunctionBytes(symbol->address);
        offset_location offset = m_loader->offset(symbol->address);

        if(view.eob() || !offset.valid)
            return true;

        sigdb.search(view, [&](const json& signature) {
            String signame = signature["name"];
            this->document()->lock(symbol->address, signame, signature["symboltype"]);
            c++;
        });

        return true;
    });

    if(c)
        r_ctx->log("Found " + String::number(c) + " signature(s)");
    else
        r_ctx->log("No signatures found");

    return true;
}

bool DisassemblerImpl::busy() const { return m_engine ? m_engine->busy() : false; }
bool DisassemblerImpl::needsWeak() const { return m_engine ? m_engine->needsWeak() : false; }

bool DisassemblerImpl::checkString(address_t fromaddress, address_t address)
{
    bool wide = false;

    if(this->locationIsString(address, &wide) < MIN_STRING)
        return false;

    if(wide)
    {
        this->document()->symbol(address, SymbolType::WideString);
        this->document()->autoComment(fromaddress, "WIDE STRING: " + this->readWString(address).quoted());
    }
    else
    {
        this->document()->symbol(address, SymbolType::String);
        this->document()->autoComment(fromaddress, "STRING: " + this->readString(address).quoted());
    }

    this->pushReference(address, fromaddress);
    return true;
}

bool DisassemblerImpl::readAddress(address_t address, size_t size, u64 *value) const
{
    if(!value) return false;

    const Segment* segment = r_docnew->segment(address);
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
    if(!value)
        return false;

    return this->readAddress(address, m_assembler->addressWidth(), value);
}

void DisassemblerImpl::disassemble(address_t address)
{
    m_engine->enqueue(address);

    if(m_engine->busy())
        return;

    m_engine->execute(DisassemblerEngineSteps::Algorithm);
}

void DisassemblerImpl::popTarget(address_t address, address_t pointedby) { m_referencetable.popTarget(address, pointedby); }
void DisassemblerImpl::pushTarget(address_t address, address_t pointedby) { m_referencetable.pushTarget(address, pointedby); }
void DisassemblerImpl::pushReference(address_t address, address_t refby) { m_referencetable.push(address, refby); }

void DisassemblerImpl::checkLocation(address_t fromaddress, address_t address)
{
    Segment* segment = r_docnew->segment(address);
    if(!segment) return;

    const Symbol* symbol = r_docnew->symbol(address);

    if(symbol && symbol->is(SymbolType::StringNew))
    {
        if(symbol->hasFlag(SymbolFlags::WideString))
            r_docnew->autoComment(fromaddress, "WIDE STRING: " + this->readWString(address).quoted());
        else
            r_docnew->autoComment(fromaddress, "STRING: " + this->readString(address).quoted());
    }
    else
        r_docnew->data(address, r_asm->addressWidth());

    this->pushReference(address, fromaddress);
}

void DisassemblerImpl::computeBasicBlocks()
{
    auto lock = x_lock_safe_ptr(m_loader->document());
    ListingFunctions* lf = lock->functions();
    lf->invalidateGraphs();

    for(size_t i = 0; i < this->documentNew()->segments()->size(); i++)
    {
        //Segment* s = this->documentNew()->segments()->at(i);
        //s->coveragebytes = REDasm::npos;
    }

    //for(size_t i = 0; i < lf->size(); i++)
        //this->computeBasicBlocks(lock, lf->at(i));
}

void DisassemblerImpl::disassemble()
{
    m_engine = std::make_unique<DisassemblerEngine>();
    m_starttime = std::chrono::steady_clock::now();

    if(!this->documentNew()->segmentsCount()) return;

    const ListingFunctions* functions = r_docnew->functions();

    // Preload functions for analysis
    for(size_t i = 0; i < functions->size(); i++)
        m_engine->enqueue(functions->at(i));

    if(r_ctx->hasFlag(ContextFlags::StepDisassembly)) r_ctx->log("Stepped disassembly, press F8");
    else if(m_engine->concurrency() == 1) r_ctx->log("Single threaded disassembly");
    else r_ctx->log("Disassembling with " + String::number(m_engine->concurrency()) + " threads");

    m_engine->stepCompleted.connect(this, [&](EventArgs*) { PIMPL_Q(Disassembler); q->busyChanged(); });
    m_engine->execute();
}

void DisassemblerImpl::stop() { if(m_engine) m_engine->stop(); }
void DisassemblerImpl::pause() { if(m_engine) m_engine->pause(); }
void DisassemblerImpl::resume() { if(m_engine) m_engine->resume(); }

void DisassemblerImpl::computeBasicBlocks(document_x_lock &lock, address_t address)
{
    return;
    r_ctx->status("Computing basic blocks @ " + String::hex(address));
    auto g = std::make_unique<FunctionGraph>();

    if(!g->build(address))
        return;

    Segment* segment = r_doc->segment(address);

    if(segment)
    {
        if(segment->coveragebytes == REDasm::npos)
            segment->coveragebytes = g->bytesCount();
        else
            segment->coveragebytes += g->bytesCount();
    }

    const NodeList& nodes = g->nodes();

    // Add basic block separators to listing
    for(size_t i = 0; (nodes.size() > 1) && (i < nodes.size() - 1); i++)
    {
        const FunctionBasicBlock* fbb = variant_object<FunctionBasicBlock>(g->data(nodes.at(i)));

        if(!fbb)
            continue;

        const ListingItem* item = fbb->instructionEndItem();

        if(!item)
            continue;

        lock->separator(item->address_new);
    }

    lock->functions()->graph(address, g.release());
}

} // namespace REDasm
