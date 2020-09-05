#include "engine.h"
#include "../support/utils.h"
#include "../eventdispatcher.h"
#include "../disassembler.h"
#include "../context.h"
#include "../builtin/analyzer/functionanalyzer.h"
#include "../builtin/graph/functiongraph.h"
#include "gibberish/gibberishdetector.h"
#include "stringfinder.h"
#include <vector>

Engine::Engine(Disassembler* disassembler): m_disassembler(disassembler), m_algorithm(disassembler->algorithm()) { GibberishDetector::initialize(); }
Engine::~Engine() { this->stop(); }
size_t Engine::currentStep() const { return m_currentstep; }
void Engine::reset() { m_currentstep = Engine::State_None; }

void Engine::execute()
{
    while(m_currentstep < State_Last)
    {
        switch(m_currentstep)
        {
            case Engine::State_None:      m_sigcount = 0; m_currentstep++; break;
            case Engine::State_Algorithm: this->algorithmStep();  break;
            case Engine::State_CFG:       this->cfgStep();        break;
            case Engine::State_Analyze:   this->analyzeStep();    break;
            case Engine::State_Signature: this->signatureStep();  break;
            default:                      rd_ctx->log("Unknown step: " + Utils::number(m_currentstep)); return;
        }
    }

    if(!m_algorithm->hasNext())
    {
        this->notify(false);
        rd_ctx->log("Analysis completed");
        rd_ctx->status("Ready");
    }
    else // More addresses pending: run Algorithm again
        this->execute(Engine::State_Algorithm);
}

void Engine::execute(size_t step)
{
    if(step == m_currentstep) return;

    this->setStep(step);
    this->execute();
}

bool Engine::cfg(rd_address address)
{
    size_t idx = m_disassembler->document()->functionIndex(address);
    if(idx == RD_NPOS) return false;

    this->generateCfg(idx);
    return true;
}

void Engine::setStep(size_t step) { m_currentstep = step; }

bool Engine::needsWeak() const
{
    switch(m_currentstep)
    {
        case Engine::State_Algorithm: return true;
        default: break;
    }

    return false;
}

bool Engine::busy() const { return m_busy; }
void Engine::stop() { this->notify(false); }

void Engine::algorithmStep()
{
    //m_signatures = r_ldr->signatures(); // Preload signatures

    if(!m_algorithm->hasNext()) return; // Ignore spurious disassemble requests
    this->notify(true);
    m_algorithm->disassemble();
    this->nextStep();
}

void Engine::analyzeStep()
{
    rd_ctx->status("Analyzing...");

    for(const RDAnalyzerPlugin* p : rd_ctx->selectedAnalyzers())
    {
        if(HAS_FLAG(p, AnalyzerFlags_RunOnce) && m_analyzecount.count(p)) continue;

        m_analyzecount[p]++;
        if(p->execute) p->execute(p, CPTR(RDDisassembler, m_disassembler));
    }

    if(!m_algorithm->hasNext())
    {
        this->cfgStep(); // Run CFG again
        this->nextStep();
        return;
    }

    this->setStep(State_Algorithm); // Repeat algorithm
}

void Engine::cfgStep()
{
    rd_ctx->status("Generating CFG...");
    m_disassembler->document()->invalidateGraphs();

    for(size_t i = 0; i < m_disassembler->document()->functionsCount(); i++)
        this->generateCfg(i);

    this->nextStep();
}

void Engine::signatureStep()
{
    //TODO: Stub
    this->nextStep();
}

void Engine::generateCfg(size_t funcindex)
{
    RDLocation loc = m_disassembler->document()->functionAt(funcindex);
    rd_ctx->status("Computing basic blocks @ " + Utils::hex(loc.address));
    auto g = std::make_unique<FunctionGraph>(m_disassembler);
    bool cfgdone = false;

    // Build CFG
    cfgdone = g->build(loc.address);

    if(cfgdone) // Apply CFG
    {
        const RDGraphNode* nodes = nullptr;
        size_t c = g->nodes(&nodes);

        // // Add basic block separators to listing
        for(size_t i = 0; (c > 1) && (i < c - 1); i++)
        {
            const FunctionBasicBlock* fbb = reinterpret_cast<const FunctionBasicBlock*>(g->data(nodes[i])->p_data);
            if(!fbb) continue;

            RDDocumentItem item;
            if(!fbb->getEndItem(&item)) continue;

            m_disassembler->document()->separator(item.address);
        }

        m_disassembler->document()->graph(g.release());
    }
    else
        rd_ctx->problem("Graph creation failed @ " + Utils::hex(loc.address));
}

void Engine::notify(bool busy)
{
    m_busy = busy;
    EventDispatcher::enqueue<RDEventArgs>(RDEvents::Event_BusyChanged, this);
}

void Engine::nextStep() { m_currentstep++; }
