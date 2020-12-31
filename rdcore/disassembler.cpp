#include "disassembler.h"
#include "support/error.h"
#include "support/utils.h"
#include "context.h"
#include "document/document.h"
#include "builtin/graph/functiongraph.h"
#include "support/utils.h"
#include <deque>

Disassembler::Disassembler(Context* ctx): Object(ctx) { }
Assembler* Disassembler::assembler() const { return m_assembler.get(); }
Loader* Disassembler::loader() const { return m_loader.get(); }
SafeAlgorithm& Disassembler::algorithm() { return m_algorithm; }
bool Disassembler::needsWeak() const { return m_engine ? m_engine->needsWeak() : false; }
bool Disassembler::busy() const { return m_engine ? m_engine->busy() : false; }
void Disassembler::enqueue(rd_address address) { m_algorithm->enqueue(address); }

bool Disassembler::disassembleFunction(rd_address address, const char* name)
{
    if(!this->document()->function(address, name ? name : std::string())) return false;
    m_algorithm->enqueue(address);
    m_algorithm->disassemble();
    return true;
}

void Disassembler::disassembleBlock(const RDBlock* block) { m_algorithm->disassembleBlock(block); }

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

    auto& doc = this->document();

    m_engine.reset(new Engine(this->context()));
    if(doc->segments()->empty()) return;

    const SymbolTable* symboltable = doc->symbols();
    std::deque<rd_address> exporteddata; // Check Exported Data

    for(auto it = symboltable->begin(); it != symboltable->end(); it++)
    {
        const RDSymbol& symbol = it->second;

        if(IS_TYPE(&symbol, SymbolType_Data) && HAS_FLAG(&symbol, SymbolFlags_Export))
            exporteddata.push_back(symbol.address);
    }

    std::for_each(exporteddata.begin(), exporteddata.end(), [&](rd_address address) {
        this->document()->checkLocation(RD_NVAL, address);
    });

    // Preload functions for analysis
    doc->functions()->each([&](rd_address address) {
        m_algorithm->enqueue(address);
        return true;
    });

    m_engine->execute();
}

bool Disassembler::load(const MemoryBufferPtr& buffer, const std::string& filepath, const RDEntryLoader* entryloader, const RDEntryAssembler* entryassembler)
{
    m_loader = std::make_unique<Loader>(buffer, filepath, entryloader, this->context());
    m_assembler = std::make_unique<Assembler>(entryassembler, this->context());

    if(m_loader->flags() & LoaderFlags_CustomAddressing)
    {
        if(!m_loader->build()) return false;
    }
    else if(!m_loader->load())
        return false;

    m_algorithm = SafeAlgorithm(new Algorithm(this->context()));
    return true;
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

bool Disassembler::encode(RDEncodedInstruction* encoded) const { return m_assembler->encode(encoded); }

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
    return this->document()->view(startitem.address, (enditem.address - startitem.address) + 1, view);
}

SafeDocument& Disassembler::document() const { return m_loader->document(); }
DocumentNet* Disassembler::net() const { return this->document()->net(); }
MemoryBuffer* Disassembler::buffer() const { return this->document()->buffer(); }
bool Disassembler::view(rd_address address, size_t size, RDBufferView* view) const { return this->document()->view(address, size, view); }

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
