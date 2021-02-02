#include "engine.h"
#include "../support/utils.h"
#include "../context.h"
#include "../disassembler.h"
#include "../document/document.h"
#include "../config.h"
#include "../plugin/analyzer.h"
#include "../builtin/analyzer/functionanalyzer.h"
#include "../builtin/graph/functiongraph/functiongraph.h"
#include "gibberish/gibberishdetector.h"
#include "stringfinder.h"
#include <vector>

Engine::Engine(Context* ctx): Object(ctx), m_algorithm(ctx->disassembler()->algorithm()) { GibberishDetector::initialize(); }
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
            default:                      rd_cfg->log("Unknown step: " + Utils::number(m_currentstep)); return;
        }
    }

    if(!m_algorithm->hasNext())
    {
        this->notify(false);
        rd_cfg->log("Analysis completed");
        rd_cfg->status("Ready");
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
    auto loc = this->context()->document()->functionStart(address);
    if(!loc.valid) return false;
    this->generateCfg(loc.address);
    return true;
}

void Engine::setStep(size_t step) { m_currentstep = step; }

bool Engine::needsWeak() const
{
    switch(m_currentstep)
    {
        case Engine::State_Algorithm:
        case Engine::State_Analyze:
            return true;

        default: break;
    }

    return false;
}

bool Engine::busy() const { return m_busy; }
void Engine::stop() { if(m_busy) this->notify(false); }

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
    rd_cfg->status("Analyzing...");
    const FunctionContainer* functions = this->context()->document()->functions();

    size_t oldfc = functions->size();
    this->analyzeAll();

    if(!m_algorithm->hasNext())
    {
        // Functions count is changed, trigger analysis again
        while(oldfc != functions->size())
        {
            oldfc = functions->size();
            this->analyzeAll();
        }

        this->cfgStep(); // Run CFG again
        this->nextStep();
        return;
    }

    this->setStep(State_Algorithm); // Repeat algorithm
}

void Engine::cfgStep()
{
    rd_cfg->status("Generating CFG...");
    this->context()->document()->invalidateGraphs();

    const FunctionContainer* functions = this->context()->document()->functions();
    DocumentNet* net = this->context()->net();

    this->context()->status("Processing function bounds");

    functions->each([&](rd_address address) {
        net->unlinkPrev(address);
        return true;
    });

    this->context()->status("Computing basic blocks");

    functions->each([&](rd_address address) {
        this->generateCfg(address);
        return true;
    });

    this->nextStep();
}

void Engine::signatureStep()
{
    //TODO: Stub
    this->nextStep();
}

void Engine::analyzeAll()
{
    for(const Analyzer* p : this->context()->selectedAnalyzers())
    {
        if(HAS_FLAG(p->plugin(), AnalyzerFlags_RunOnce) && m_analyzecount.count(p)) continue;

        m_analyzecount[p]++;
        p->execute();
    }
}

void Engine::generateCfg(rd_address address)
{
    auto g = std::make_unique<FunctionGraph>(this->context());
    bool cfgdone = false;

    // Build CFG
    cfgdone = g->build(address);

    if(cfgdone) // Apply CFG
    {
        auto& doc = this->context()->document();
        const RDGraphNode* nodes = nullptr;
        size_t tailaddress = 0, c = g->nodes(&nodes);

        // Add basic block separators to listing
        for(size_t i = 0; (c > 1) && (i < c - 1); i++)
        {
            const FunctionBasicBlock* fbb = reinterpret_cast<const FunctionBasicBlock*>(g->data(nodes[i])->p_data);
            if(!fbb) continue;

            RDDocumentItem item;
            if(!fbb->getEndItem(&item)) continue;
            if(item.address > tailaddress) tailaddress = item.address;
            doc->separator(item.address);
        }

        if(!tailaddress && c) // Try to get the first block
        {
            const FunctionBasicBlock* fbb = reinterpret_cast<const FunctionBasicBlock*>(g->data(nodes[0])->p_data);

            RDDocumentItem item;
            if(fbb && fbb->getEndItem(&item)) tailaddress = item.address;
        }

        if(tailaddress) doc->empty(tailaddress);
        doc->graph(g.release());
    }
    else
        this->context()->problem("Graph creation failed @ " + Utils::hex(address));
}

void Engine::notify(bool busy)
{
    m_busy = busy;
    this->context()->notify<RDEventArgs>(RDEvents::Event_BusyChanged, this);
}

void Engine::nextStep() { m_currentstep++; }
