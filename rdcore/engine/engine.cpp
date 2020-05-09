#include "engine.h"
#include "../support/utils.h"
#include "../eventdispatcher.h"
#include "../disassembler.h"
#include "../context.h"
#include "../builtin/graph/functiongraph.h"
#include "gibberish/gibberishdetector.h"
#include "stringfinder.h"
#include "analyzer.h"
#include <rdapi/disassembler.h>

Engine::Engine(Disassembler* disassembler): m_disassembler(disassembler), m_algorithm(disassembler->algorithm())
{
    GibberishDetector::initialize();
    if(!rd_ctx->sync()) JobManager::initialize();

    m_analyzer.reset(new Analyzer(disassembler));
}

Engine::~Engine() { this->stop(); }
size_t Engine::currentStep() const { return m_currentstep; }
size_t Engine::concurrency() const { return JobManager::concurrency(); }
void Engine::reset() { m_currentstep = Engine::EngineState_None; }

void Engine::execute()
{
    if(!rd_ctx->sync() && !JobManager::initialized()) return;
    if(m_currentstep == Engine::EngineState_None) m_sigcount = 0;

    size_t newstep = ++m_currentstep;

    if(newstep >= Engine::EngineState_Last)
    {
        if(!m_algorithm->hasNext())
        {
            this->notify(false);
            rd_ctx->status("Ready");
        }
        else // More addresses pending: run Algorithm again
            this->execute(Engine::EngineState_Algorithm);
    }
    else
        this->execute(newstep);
}

void Engine::execute(size_t step)
{
    m_currentstep = step;

    switch(step)
    {
        case Engine::EngineState_None:       return;
        case Engine::EngineState_Strings:    this->stringsStep();    break;
        case Engine::EngineState_Algorithm:  this->algorithmStep();  break;
        case Engine::EngineState_Unexplored: this->unexploredStep(); break;
        case Engine::EngineState_Analyze:    this->analyzeStep();    break;
        case Engine::EngineState_CFG:        this->cfgStep();        break;
        case Engine::EngineState_Signature:  this->signatureStep();  break;
        default:                             rd_ctx->log("Unknown step: " + Utils::number(step)); return;
    }
}

bool Engine::needsWeak() const
{
    switch(m_currentstep)
    {
        case Engine::EngineState_Strings:
        case Engine::EngineState_Algorithm:
        case Engine::EngineState_Unexplored:
            return true;

        default: break;
    }

    return false;
}

bool Engine::busy() const { return m_busy; }

void Engine::stop()
{
    if(!rd_ctx->sync()) JobManager::deinitialize();
    this->notify(false);
}

void Engine::stringsStep()
{
    m_starttime = std::chrono::steady_clock::now();
    this->notify(true);

    if(rd_ctx->sync())
    {
        for(size_t i = 0; i < rd_doc->segmentsCount(); i++)
            this->stringsJob({i, 0});

        this->execute();
    }
    else
        JobManager::dispatch(rd_doc->segmentsCount(), this, &Engine::stringsJob);
}

void Engine::algorithmStep()
{
    //m_signatures = r_ldr->signatures(); // Preload signatures

    if(rd_ctx->sync()) this->algorithmJobSync();
    else JobManager::dispatch(this, &Engine::algorithmJob);
}

void Engine::analyzeStep()
{
    if(rd_ctx->flags() & ContextFlag_DisableAnalyzer)
    {
        this->execute();
        return;
    }

    if(rd_ctx->sync()) this->analyzeJob();
    else JobManager::execute(this, &Engine::analyzeJob);
}

void Engine::unexploredStep()
{
    if(rd_ctx->flags() & ContextFlag_DisableUnexplored)
    {
        this->execute();
        return;
    }

    this->execute();
}

void Engine::cfgStep()
{
    if(rd_ctx->flags() & ContextFlag_DisableCFG)
    {
        this->execute();
        return;
    }

    m_disassembler->document()->invalidateGraphs();
    rd_ctx->status("Generating CFG...");

    //for(size_t i = 0; i < r_doc->segmentsCount(); i++)
        //r_doc->segmentCoverageAt(i, REDasm::npos);

    if(rd_ctx->sync())
    {
        this->cfgJobSync();
        return;
    }

    JobManager::dispatch(rd_doc->functionsCount(), JobManager::concurrency(), this, &Engine::cfgJob);
}

void Engine::signatureStep()
{
    //TODO: Stub
    this->execute();
}

void Engine::stringsJob(const JobDispatchArgs& args)
{
    this->searchStringsAt(args.jobindex);

    if(JobManager::last())
        this->execute();
}

void Engine::algorithmJob(const JobDispatchArgs&)
{
    Utils::yloop([&]() -> bool {
                     if(!JobManager::initialized() || !m_algorithm->hasNext()) return false;
                     m_algorithm->next();
                     return true;
                 });

    if(JobManager::last())
        this->execute();
}

void Engine::analyzeJob()
{
    rd_ctx->status("Analyzing...");
    m_analyzer->analyze();

    auto duration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - m_starttime);
    if(duration.count()) rd_ctx->log("Analysis completed in ~" + Utils::number(duration.count()) + " second(s)");
    else rd_ctx->log("Analysis completed");

    if(m_algorithm->hasNext()) this->execute(EngineState_Algorithm); // Repeat algorithm
    else this->execute();
}

void Engine::cfgJob(const JobDispatchArgs& args)
{
    if(args.jobindex >= m_disassembler->document()->functionsCount()) return;

    RDLocation loc = m_disassembler->document()->functionAt(args.jobindex);
    rd_ctx->status("Computing basic blocks @ " + Utils::hex(loc.address));
    auto g = std::make_unique<FunctionGraph>(m_disassembler);
    bool cfgdone = false;

    { // Build CFG
        auto lock = s_lock_safe_ptr(m_disassembler->document());
        cfgdone = g->build(loc.address);
    }

    if(cfgdone) // Apply CFG
    {
        auto lock = x_lock_safe_ptr(m_disassembler->document());
        //lock->segmentCoverage(address, g->bytesCount());

        const RDGraphNode* nodes = nullptr;
        size_t c = g->nodes(&nodes);

        // // Add basic block separators to listing
        for(size_t i = 0; (c > 1) && (i < c - 1); i++)
        {
            const FunctionBasicBlock* fbb = reinterpret_cast<const FunctionBasicBlock*>(g->data(nodes[i])->p_data);
            if(!fbb) continue;

            RDDocumentItem item;
            if(!fbb->getEndItem(&item)) continue;

            lock->separator(item.address);
        }

        lock->graph(g.release());
    }
    else
        rd_ctx->problem("Graph creation failed @ " + Utils::hex(loc.address));

    if(JobManager::last())
        this->execute();
}

void Engine::signatureJob(const JobDispatchArgs& args)
{
}

void Engine::stringsJobSync()
{
    for(size_t i = 0; i < rd_doc->segmentsCount(); i++)
        this->searchStringsAt(i);

    this->execute();
}

void Engine::algorithmJobSync()
{
    while(m_algorithm->hasNext())
        m_algorithm->next();

    this->execute();
}

void Engine::cfgJobSync()
{
    for(size_t i = 0; i < rd_doc->functionsCount(); i++)
        this->cfgJob({i, 0});

    this->execute();
}

void Engine::searchStringsAt(size_t index) const
{
    if(index >= rd_doc->segmentsCount()) return;

    RDSegment segment;
    rd_doc->segmentAt(index, &segment);

    if(HAS_FLAG(&segment, SegmentFlags_Bss)) return;

    StringFinder sf(segment);
    sf.find();
}

void Engine::notify(bool busy)
{
    m_busy = busy;
    EventDispatcher::dispatch<RDEventArgs>(RDEvents::Event_DisassemblerBusyChanged, this);
}
