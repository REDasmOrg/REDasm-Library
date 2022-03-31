#include "disassembler.h"
#include "support/error.h"
#include "support/utils.h"
#include "context.h"
#include "database/addressdatabase.h"
#include "document/document.h"
#include "builtin/graph/functiongraph/functiongraph.h"
#include "support/utils.h"
#include <deque>

Disassembler::Disassembler(Context* ctx): Object(ctx) { }
Assembler* Disassembler::assembler() const { return m_assembler.get(); }
Loader* Disassembler::loader() const { return m_loader.get(); }
SafeAlgorithm& Disassembler::algorithm() { return m_algorithm; }
bool Disassembler::isWeak() const { return m_engine ? m_engine->isWeak() : false; }
bool Disassembler::busy() const { return m_engine ? m_engine->busy() : false; }
void Disassembler::enqueue(rd_address address) { m_algorithm->enqueue(address); }

bool Disassembler::disassembleFunction(rd_address address)
{
    this->disassembleAt(address);
    return m_engine->cfg(address);
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
        if(!m_engine->busy())
            m_engine->execute(Engine::State_Algorithm);

        return;
    }

    auto& doc = this->document();
    m_engine.reset(new Engine(this->context()));
    if(!doc->getSegments(nullptr)) return;

    const rd_address* addresses = nullptr;
    size_t c = doc->getLabelsByFlag(AddressFlags_Exported, &addresses);
    std::vector<rd_address> exporteddata; // Copy Exports
    exporteddata.assign(addresses, addresses + c);
    for(rd_address address : exporteddata) this->document()->checkLocation(RD_NVAL, address);

    c = doc->getFunctions(&addresses); // Preload functions for analysis
    for(size_t i = 0; i < c; i++) m_algorithm->enqueue(addresses[i]);

    m_engine->execute();
}

void Disassembler::stop() { if(m_engine) m_engine->stop(); }

const char* Disassembler::getFunctionHexDump(rd_address address, rd_address* resaddress) const
{
    static std::string hexdump;

    RDBufferView view;
    if(!this->getFunctionBytes(address, &view)) return nullptr;

    if(resaddress)
    {
        auto loc = this->document()->getFunctionStart(address);
        *resaddress = loc.valid ? loc.address : RD_NVAL;
    }

    hexdump = Utils::hexString(&view);
    return hexdump.c_str();
}

void Disassembler::setWeak(bool b) { if(m_engine) m_engine->setWeak(b); }
bool Disassembler::encode(RDEncodedInstruction* encoded) const { return m_assembler->encode(encoded); }

bool Disassembler::getFunctionBytes(rd_address& address, RDBufferView* view) const
{
    RDLocation loc = this->document()->getFunctionStart(address);
    if(!loc.valid) return { };

    const auto* graph = this->document()->getGraph(loc.address);
    if(!graph) return { };

    const RDGraphNode* nodes = nullptr;
    size_t c = graph->nodes(&nodes);

    rd_address startaddress = RD_NVAL, endaddress = RD_NVAL;

    for(size_t i = 0; i < c; i++)
    {
        const auto* fbb = reinterpret_cast<const FunctionBasicBlock*>(graph->data(nodes[i])->p_data);
        if(!fbb) return { };

        if((startaddress == RD_NVAL) || (startaddress > fbb->startaddress))
            startaddress = fbb->startaddress;

        if((endaddress == RD_NVAL) || (endaddress < fbb->endaddress))
            endaddress = fbb->endaddress;
    }

    if((startaddress == RD_NVAL) || (endaddress == RD_NVAL)) return { };

    address = loc.address;
    return this->document()->getView(startaddress, (endaddress - startaddress) + 1, view);
}

SafeDocument& Disassembler::document() const { return m_loader->document(); }

void Disassembler::prepare(const MemoryBufferPtr& buffer, const std::string& filepath, const RDEntryLoader* entryloader, const RDEntryAssembler* entryassembler)
{
    m_loader = std::make_unique<Loader>(buffer, filepath, entryloader, this->context());
    m_assembler = std::make_unique<Assembler>(entryassembler, this->context());
}

bool Disassembler::load(const RDLoaderBuildParams* buildparams)
{
    this->addressDatabase()->pushAssembler(m_assembler->id());

    m_algorithm = SafeAlgorithm(new Algorithm(this->context()));

    if(m_loader->flags() & LoaderFlags_CustomAddressing)
    {
        if(!m_loader->build(buildparams)) return false;
    }
    else if(!m_loader->load())
        return false;

    return true;
}
