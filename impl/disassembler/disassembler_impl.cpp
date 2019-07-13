#include "disassembler_impl.h"
#include <redasm/disassembler/listing/listingfunctions.h>
#include <redasm/plugins/assembler/assembler.h>
#include <redasm/database/signaturedb.h>
#include <redasm/graph/functiongraph.h>
#include <redasm/support/utils.h>
#include <redasm/support/path.h>
#include <redasm/context.h>
#include <cctype>

namespace REDasm {

DisassemblerImpl::DisassemblerImpl(Disassembler* q, Assembler *assembler, Loader *loader): m_pimpl_q(q), m_assembler(assembler), m_loader(loader) { }

DisassemblerImpl::~DisassemblerImpl()
{
    if(m_loader)
        m_loader->buffer()->release();
}

Loader *DisassemblerImpl::loader() const { return m_loader; }
Assembler *DisassemblerImpl::assembler() const { return m_assembler; }
const safe_ptr<ListingDocumentType> &DisassemblerImpl::document() const { return m_loader->document(); }
safe_ptr<ListingDocumentType> &DisassemblerImpl::document() { return m_loader->document(); }

ListingItemConstContainer DisassemblerImpl::getCalls(address_t address)
{
    const REDasm::ListingItem* item = this->document()->functionStart(address);

    if(!item)
        return { };

    const auto* graph = this->document()->functions()->graph(item);

    if(!graph)
        return { };

    ListingItemConstContainer calls;

    for(const auto& n : graph->nodes())
    {
        const auto* fbb = graph->data(n);

        if(!fbb)
            continue;

        for(size_t i = fbb->startIndex(); i <= fbb->endIndex(); i++)
        {
            const ListingItem* item = this->document()->itemAt(i);

            if(!item->is(ListingItemType::InstructionItem))
                continue;

            CachedInstruction instruction = this->document()->instruction(item->address());

            if(instruction->is(InstructionType::Call))
                calls.insert(item);
        }
    }

    return calls;
}

ReferenceTable *DisassemblerImpl::references() { return &m_referencetable; }
ReferenceVector DisassemblerImpl::getReferences(address_t address) const { return m_referencetable.referencesToVector(address); }
ReferenceSet DisassemblerImpl::getTargets(address_t address) const { return m_referencetable.targets(address); }

BufferView DisassemblerImpl::getFunctionBytes(address_t address)
{
    const REDasm::ListingItem* item = this->document()->functionStart(address);

    if(!item)
        return BufferView();

    const auto* graph = this->document()->functions()->graph(item);

    if(!graph)
        return BufferView();

    ListingItemConstContainer instructions;
    size_t startidx = REDasm::npos, endidx = REDasm::npos;

    for(const auto& n : graph->nodes())
    {
        const auto* fbb = graph->data(n);

        if(!fbb)
            continue;

        if(startidx == REDasm::npos)
            startidx = fbb->startIndex();
        else if(startidx > fbb->startIndex())
            startidx = fbb->startIndex();

        if(endidx == REDasm::npos)
            endidx = fbb->endIndex();
        else if(endidx < fbb->endIndex())
            endidx = fbb->endIndex();
    }

    if((startidx == REDasm::npos) | (endidx == REDasm::npos))
        return BufferView();

    const ListingItem* startitem = this->document()->itemAt(startidx);
    const ListingItem* enditem = this->document()->itemAt(endidx);

    if(!startitem || !enditem)
        return BufferView();

    BufferView v = this->loader()->view(startitem->address());
    v.resize(enditem->address() - startitem->address());
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

JobState DisassemblerImpl::state() const { return m_jobs.state(); }

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

    const REDasm::Symbol* symbol = this->document()->symbol(item->address());

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

bool DisassemblerImpl::busy() const { return m_analyzejob.active() || m_jobs.active(); }

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
    m_algorithm->enqueue(address);

    if(m_jobs.active())
        return;

    this->disassembleJob();
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
    m_starttime = std::chrono::steady_clock::now();

    if(!this->document()->segmentsCount())
    {
        r_ctx->log("ERROR: Segment list is empty");
        return;
    }

    const SymbolTable* symboltable = this->document()->symbols();

    // Preload loader functions for analysis
    symboltable->iterate(SymbolType::FunctionMask, [=](const Symbol* symbol) -> bool {
        m_algorithm->enqueue(symbol->address);
        return true;
    });

    const Symbol* entrypoint = this->document()->documentEntry();

    if(entrypoint)
        m_algorithm->enqueue(entrypoint->address); // Push entry point

    r_ctx->log("Disassembling with " + String::number(m_jobs.concurrency()) + " threads");
    this->disassembleJob();
}

void DisassemblerImpl::stop() { m_jobs.stop(); }
void DisassemblerImpl::pause() { m_jobs.pause(); }
void DisassemblerImpl::resume() { m_jobs.resume(); }
void DisassemblerImpl::disassembleJob() { m_jobs.work(std::bind(&DisassemblerImpl::disassembleStep, this, std::placeholders::_1)); }

void DisassemblerImpl::disassembleStep(Job *job)
{
    if(m_algorithm->hasNext())
        m_algorithm->next();
    else
        job->stop();

    if(!m_jobs.active())
        m_analyzejob.start();
}

void DisassemblerImpl::analyzeStep()
{
    m_algorithm->analyze();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - m_starttime);

    if(duration.count())
        r_ctx->log("Analysis completed in ~" + String::number(duration.count()) + " second(s)");
    else
        r_ctx->log("Analysis completed");
}

void DisassemblerImpl::computeBasicBlocks(document_x_lock &lock, const ListingItem *functionitem)
{
    PIMPL_Q(Disassembler);
    r_ctx->status("Computing basic blocks @ " + String::hex(functionitem->address()));
    auto g = std::make_unique<Graphing::FunctionGraph>(q);

    if(!g->build(functionitem))
        return;

    lock->functions()->graph(functionitem, g.release());
}

} // namespace REDasm
