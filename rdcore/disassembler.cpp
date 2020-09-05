#include "disassembler.h"
#include "support/error.h"
#include "support/utils.h"
#include "eventdispatcher.h"
#include "context.h"
#include "builtin/graph/functiongraph.h"
#include "support/utils.h"

Disassembler::Disassembler(const RDLoaderRequest* request, RDLoaderPlugin* ploader, RDAssemblerPlugin* passembler)
{
    rd_ctx->setDisassembler(this);
    m_loader = std::make_unique<Loader>(ploader, request);
    m_assembler = std::make_unique<Assembler>(passembler, this);
    m_algorithm = SafeAlgorithm(new Algorithm(this));
}

Disassembler::~Disassembler() { EventDispatcher::unsubscribeAll(); }
Assembler* Disassembler::assembler() const { return m_assembler.get(); }
Loader* Disassembler::loader() const { return m_loader.get(); }
SafeAlgorithm& Disassembler::algorithm() { return m_algorithm; }
bool Disassembler::needsWeak() const { return m_engine ? m_engine->needsWeak() : false; }
bool Disassembler::busy() const { return m_engine ? m_engine->busy() : false; }
void Disassembler::enqueue(rd_address address) { m_algorithm->enqueue(address); }
void Disassembler::schedule(rd_address address) { m_algorithm->schedule(address); }

void Disassembler::disassembleAt(rd_address address)
{
    m_algorithm->enqueue(address);
    m_algorithm->disassemble();
}

void Disassembler::disassemble()
{
    if(m_engine) // Just wake up the engine, if not busy
    {
        if(!m_engine->busy()) m_engine->execute(Engine::State_Algorithm);
        return;
    }

    m_engine.reset(new Engine(this));
    if(!this->document()->segmentsCount()) return;

    // Preload functions for analysis
    for(size_t i = 0; i < this->document()->functionsCount(); i++)
        m_algorithm->enqueue(this->document()->functionAt(i).address);

    m_engine->execute();
}

void Disassembler::stop() { if(m_engine) m_engine->stop(); }

const char* Disassembler::getFunctionHexDump(rd_address address, RDSymbol* symbol) const
{
    static std::string hexdump;

    RDBufferView view;
    if(!this->getFunctionBytes(address, &view)) return nullptr;
    if(symbol && !this->document()->symbol(address, symbol)) return nullptr;

    hexdump = Utils::hexString(&view);
    return hexdump.c_str();
}

const char* Disassembler::getHexDump(rd_address address, size_t size) const
{
    static std::string hexdump;

    RDBufferView view;
    if(!m_loader->view(address, size, &view)) return nullptr;

    hexdump = Utils::hexString(&view);
    return hexdump.c_str();
}

const char16_t* Disassembler::readWString(rd_address address, size_t* len) const { return this->readStringT<char16_t>(address, len); }
const char* Disassembler::readString(rd_address address, size_t* len) const { return this->readStringT<char>(address, len); }

std::string Disassembler::readWString(rd_address address, size_t len) const
{
    const char16_t* s = this->readWString(address, &len);
    return s ? Utils::toString(std::u16string(s, len)) : std::string();
}

std::string Disassembler::readString(rd_address address, size_t len) const
{
    const char* s = this->readString(address, &len);
    return s ? std::string(s, len) : std::string();
}

RDLocation Disassembler::dereference(rd_address address) const
{
    RDLocation loc;
    loc.valid = this->readAddress(address, m_assembler->addressWidth(), &loc.value);
    return loc;
}

void Disassembler::markLocation(rd_address fromaddress, rd_address address)
{
    if(!this->document()->segment(address, nullptr)) return;

    if(this->document()->symbol(address, nullptr))
    {
        m_net.addRef(fromaddress, address); // Just add the reference
        return;
    }

    RDBufferView view;

    if(m_loader->view(address, &view))
    {
        size_t totalsize = 0;
        rd_flag flags = StringFinder::categorize(&view, &totalsize);

        if(StringFinder::checkAndMark(this, address, flags, totalsize))
        {
            if(flags & SymbolFlags_AsciiString) this->document()->autoComment(fromaddress, "STRING: " + Utils::quoted(this->readString(address)));
            else if(flags & SymbolFlags_WideString) this->document()->autoComment(fromaddress, "WIDE STRING: " + Utils::quoted(this->readWString(address)));
            else REDasmError("Unhandled String symbol", address);
        }
        else
        {
            RDLocation loc = this->dereference(address);
            if(loc.valid && this->document()->symbol(loc.address, nullptr)) this->markPointer(address, fromaddress); // It points to another symbol
            else this->document()->data(address, m_assembler->addressWidth(), std::string());
        }
    }
    else // This address belongs to a memory mapped only area
        this->document()->data(address, m_assembler->addressWidth(), std::string());

    m_net.addRef(fromaddress, address);
}

rd_type Disassembler::markPointer(rd_address address, rd_address fromaddress)
{
    // RDLocation loc = this->dereference(address);
    // if(!loc.valid) return this->markLocation(address, fromaddress);

    // this->document()->pointer(address, SymbolType_Data, std::string());

    // RDSymbol symbol;
    // if(!this->document()->symbol(loc.address, &symbol)) return SymbolType_None;

    // const char* symbolname = this->document()->name(symbol.address);
    // if(!symbolname) return SymbolType_None;

    // if(IS_TYPE(&symbol, SymbolType_String))
    // {
    //     if(HAS_FLAG(&symbol, SymbolFlags_WideString)) this->document()->autoComment(fromaddress, std::string("=> ") + symbolname + ": " + Utils::quoted(this->readWString(loc.address)));
    //     else this->document()->autoComment(fromaddress, std::string("=> ") +  symbolname + ": " + Utils::quoted(this->readString(loc.address)));
    // }
    // else if(HAS_FLAG(&symbol, SymbolType_Import))
    //     this->document()->autoComment(fromaddress, std::string("=> IMPORT: ") + symbolname);
    // else if(HAS_FLAG(&symbol, SymbolFlags_Export))
    //     this->document()->autoComment(fromaddress, std::string("=> EXPORT: ") + symbolname);
    // else
    //     return SymbolType_None;

    // this->pushReference(loc.address, fromaddress);
    return SymbolType_Data;
}

size_t Disassembler::markTable(rd_address startaddress, rd_address fromaddress, size_t count)
{
    return 0;
    // rd_ctx->statusAddress("Checking address table", startaddress);

    // RDSymbol symbol;
    // if(this->document()->symbol(startaddress, &symbol) && HAS_FLAG(&symbol, SymbolFlags_TableItem)) return RD_NPOS;

    // rd_address address = startaddress;
    // std::deque<rd_address> targets;

    // for(size_t i = 0 ; i < count; i++, address += m_assembler->addressWidth())
    // {
    //     RDLocation loc = this->dereference(address);
    //     if(!loc.valid) break;

    //     rd_type currsymboltype = this->markLocation(loc.address, address);
    //     if(currsymboltype == SymbolType_None) break;

    //     this->pushReference(address, fromaddress);
    //     targets.push_back(loc.address);
    // }

    // if(targets.size() > 1)
    // {
    //     size_t i = 0;

    //     for(rd_address target : targets)
    //     {
    //         this->document()->tableItem(target, startaddress, i++);
    //         this->pushReference(target, fromaddress);
    //     }
    // }
    // else if(targets.size() == 1)
    // {
    //     this->document()->pointer(targets.front(), SymbolType_Data, std::string());
    //     this->pushReference(targets.front(), fromaddress);
    // }
    // else
    //     return 0;

    // this->document()->pointer(startaddress, SymbolType_Data, std::string());
    // return targets.size();
}

bool Disassembler::encode(RDEncodedInstruction* encoded) const { return m_assembler->encode(encoded); }

bool Disassembler::readAddress(rd_address address, size_t size, u64* value) const
{
    RDBufferView view;
    if(!m_loader->view(address, &view)) return false;

    switch(size)
    {
        case 1:  if(value) *value = *reinterpret_cast<u8*>(view.data);  break;
        case 2:  if(value) *value = *reinterpret_cast<u16*>(view.data); break;
        case 4:  if(value) *value = *reinterpret_cast<u32*>(view.data); break;
        case 8:  if(value) *value = *reinterpret_cast<u64*>(view.data); break;
        default: rd_ctx->problem("Invalid size: " + Utils::number(size)); return false;
    }

    return true;
}

bool Disassembler::getFunctionBytes(rd_address& address, RDBufferView* view) const
{
    RDLocation loc = this->document()->functionStart(address);
    if(!loc.valid) return { };

    const auto* graph = this->document()->graph(loc.address);
    if(!graph) return { };

    const RDGraphNode* nodes = nullptr;
    size_t c = graph->nodes(&nodes);

    RDDocumentItem startitem{ }, enditem{ };

    for(size_t i = 0; i < c; i++)
    {
        const auto* fbb = reinterpret_cast<const FunctionBasicBlock*>(graph->data(nodes[i])->p_data);
        if(!fbb) return { };

        if(IS_TYPE(&startitem, DocumentItemType_None) || (startitem.address > fbb->startaddress))
        {
            if(!fbb->getStartItem(&startitem)) REDasmError("Cannot find start item");
        }

        if(IS_TYPE(&enditem, DocumentItemType_None) || (enditem.address < fbb->endaddress))
        {
            if(!fbb->getEndItem(&enditem)) REDasmError("Cannot find end item");
        }
    }

    if(IS_TYPE(&startitem, DocumentItemType_None) || IS_TYPE(&enditem, DocumentItemType_None)) return { };

    address = loc.address;
    return this->loader()->view(startitem.address, (enditem.address - startitem.address) + 1, view);
}

SafeDocument& Disassembler::document() const { return m_loader->document(); }
const DocumentNet* Disassembler::net() const { return &m_net; }
DocumentNet* Disassembler::net() { return &m_net; }
MemoryBuffer* Disassembler::buffer() const { return m_loader->buffer(); }
bool Disassembler::view(rd_address address, size_t size, RDBufferView* view) const { return m_loader->view(address, size, view); }

bool Disassembler::scheduleFunction(rd_address address, const char* name)
{
    if(!this->document()->function(address, name ? name : std::string())) return false;
    m_algorithm->enqueue(address);
    return true;
}

bool Disassembler::createFunction(rd_address address, const char* name)
{
    if(!this->document()->function(address, name ? name : std::string())) return false;

    if(m_engine->currentStep() != Engine::State_Analyze)
    {
        m_algorithm->enqueue(address);
        m_engine->execute(Engine::State_Algorithm);
        return true;
    }

    // Disassemble only
    this->disassembleAt(address);
    return m_engine->cfg(address);
}
