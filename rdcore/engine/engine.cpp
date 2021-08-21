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
#include <ctime>

const std::array<const char*, Engine::State_Last> Engine::STATUS_LIST = {
    "Stop", "Algorithm", "CFG", "Analyze", "Done"
};

Engine::Engine(Context* ctx): Object(ctx), m_algorithm(ctx->disassembler()->algorithm())
{
    GibberishDetector::initialize();

    const auto& selanalyzers = this->context()->selectedAnalyzers();
    m_analyzersdone.resize(selanalyzers.size());

    for(size_t i = 0; i < selanalyzers.size(); i++)
    {
        m_analyzersnames.push_back(selanalyzers.at(i)->name());
        m_analyzersdone[i] = 0;
    }

    m_status.filepath = ctx->loader()->filePath().c_str();
    m_status.assembler = ctx->assembler()->name();
    m_status.loader = ctx->loader()->name();
    m_status.filesize = ctx->buffer()->size();
    m_status.stepslist = STATUS_LIST.data();
    m_status.stepscount = STATUS_LIST.size();
    m_status.analyzerslist = m_analyzersnames.data();
    m_status.analyzerscount = m_analyzersnames.size();
    m_status.analyzerscurrent = RD_NVAL;
    m_status.analyzersdone = m_analyzersdone.data();

    this->notifyStatus();
}

Engine::~Engine() { this->stop(); }
size_t Engine::currentStep() const { return m_status.stepscurrent; }
void Engine::reset() { m_status.stepscurrent = Engine::State_Stop; }

void Engine::execute()
{
    while(m_status.stepscurrent < State_Done)
    {
        switch(m_status.stepscurrent)
        {
            case Engine::State_Stop:
                m_status.analysisstart = static_cast<u64>(time(nullptr));
                this->nextStep();
                break;

            case Engine::State_Algorithm: this->algorithmStep(); break;
            case Engine::State_CFG:       this->cfgStep();       break;
            case Engine::State_Analyze:   this->analyzeStep();   break;
            default:                      rd_cfg->log("Unknown step: " + Utils::number(m_status.stepscurrent)); return;
        }
    }

    if(!m_algorithm->hasNext())
    {
        this->notifyBusy(false);
        rd_cfg->log("Analysis completed");
        rd_cfg->status("Ready");
    }
    else // More addresses pending: run Algorithm again
        this->execute(Engine::State_Algorithm);
}

void Engine::execute(size_t step)
{
    if(step == m_status.stepscurrent) return;

    this->setStep(step);
    this->execute();
}

bool Engine::cfg(rd_address address)
{
    auto loc = this->context()->document()->getFunctionStart(address);
    if(!loc.valid) return false;
    this->generateCfg(loc.address);
    return true;
}

void Engine::setStep(size_t step)
{
    m_status.stepscurrent = step;
    this->notifyStatus();
}

bool Engine::needsWeak() const
{
    switch(m_status.stepscurrent)
    {
        case Engine::State_Algorithm:
        case Engine::State_Analyze:
            return true;

        default: break;
    }

    return false;
}

bool Engine::busy() const { return m_status.busy; }
void Engine::stop() { if(m_status.busy) this->notifyBusy(false); }

void Engine::algorithmStep()
{
    if(!m_algorithm->hasNext()) return; // Ignore spurious disassemble requests
    this->notifyBusy(true);
    m_algorithm->disassemble();
    this->nextStep();
}

void Engine::analyzeStep()
{
    rd_cfg->status("Analyzing...");

    auto& doc = this->context()->document();
    size_t oldfc = doc->getFunctions(nullptr);

    this->analyzeAll();

    if(!m_algorithm->hasNext())
    {
        // Functions count is changed, trigger analysis again
        while(oldfc != doc->getFunctions(nullptr))
        {
            oldfc = doc->getFunctions(nullptr);
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

    const rd_address* functions = nullptr;
    size_t c = this->context()->document()->getFunctions(&functions);
    DocumentNet* net = this->context()->net();

    for(size_t i = 0; i < c; i++)
    {
        this->context()->statusAddress("Processing function bounds", functions[i]);
        net->unlinkPrev(functions[i]);
    }

    for(size_t i = 0; i < c; i++)
    {
        this->context()->statusAddress("Computing basic blocks", functions[i]);
        this->generateCfg(functions[i]);
    }

    this->nextStep();
}

void Engine::analyzeAll()
{
    const auto& analyzers = this->context()->selectedAnalyzers();

    for(size_t i = 0; i < analyzers.size(); i++)
    {
        const auto* a = analyzers.at(i);
        if(HAS_FLAG(a->plugin(), AnalyzerFlags_RunOnce) && m_analyzersdone[i]) continue;

        m_analyzersdone[i]++;
        m_status.analyzerscurrent = i;
        this->notifyStatus();
        a->execute();
    }

    m_status.analyzerscurrent = RD_NVAL;
    this->notifyStatus();
}

void Engine::generateCfg(rd_address address)
{
    auto g = std::make_unique<FunctionGraph>(this->context());

    if(g->build(address)) // Build & Apply CFG
        this->context()->document()->setGraph(g.release());
    else
        this->context()->problem("Graph creation failed @ " + Utils::hex(address));
}

void Engine::notifyStatus()
{
    auto& doc = this->context()->document();

    if(m_status.stepscurrent != m_lastnotifystep)
    {
        auto segc = doc->getSegments(nullptr);
        auto func = doc->getFunctions(nullptr);
        auto lblc = doc->getLabels(nullptr);

        m_status.segmentsdiff = segc - m_status.segmentscount;
        m_status.segmentscount = segc;
        m_status.functionsdiff = func - m_status.functionscount;
        m_status.functionscount = func;
        m_status.labelsdiff = lblc - m_status.labelscount;
        m_status.labelscount = lblc;

        m_lastnotifystep = m_status.stepscurrent;
    }

    this->context()->notify<RDAnalysisStatusEventArgs>(RDEvents::Event_AnalysisStatusChanged, this, &m_status);
}

void Engine::notifyBusy(bool busy)
{
    m_status.busy = busy;
    if(!busy) m_status.analysisend = static_cast<u64>(time(nullptr));

    this->context()->notify<RDEventArgs>(RDEvents::Event_BusyChanged, this);
    this->notifyStatus();
}

void Engine::nextStep()
{
    m_status.stepscurrent = std::min<size_t>(m_status.stepscurrent + 1, State_Done);
    this->notifyStatus();
}
