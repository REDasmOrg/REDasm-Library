#include "disassemblerengine.h"
#include <redasm/disassembler/model/functiongraph.h>
#include <redasm/disassembler/disassembler.h>
#include <redasm/plugins/assembler/assembler.h>
#include <redasm/support/event/eventmanager.h>
#include <redasm/support/utils.h>
#include <redasm/context.h>
#include "gibberish/gibberishdetector.h"
#include "stringfinder.h"

namespace REDasm {

DisassemblerEngine::DisassemblerEngine()
{
    GibberishDetector::initialize();
    if(!r_ctx->sync()) JobManager::initialize();
    m_algorithm = r_asm->createAlgorithm();
}

DisassemblerEngine::~DisassemblerEngine() { this->stop(); }
size_t DisassemblerEngine::currentStep() const { return m_currentstep; }
size_t DisassemblerEngine::concurrency() const { return JobManager::concurrency(); }
void DisassemblerEngine::reset() { m_currentstep = DisassemblerEngineSteps::None; }

void DisassemblerEngine::execute()
{
    if(!r_ctx->sync() && !JobManager::initialized()) return;
    size_t newstep = ++m_currentstep;

    if(newstep >= DisassemblerEngineSteps::Last)
    {
        r_doc->moveToEntry();

        if(m_algorithm->hasNext()) // More addresses pending: run Algorithm again
            this->execute(DisassemblerEngineSteps::Algorithm);
        else
            this->notify(false);
    }
    else
        this->execute(newstep);
}

void DisassemblerEngine::execute(size_t step)
{
    m_currentstep = step;

    switch(step)
    {
        case DisassemblerEngineSteps::None:       return;
        case DisassemblerEngineSteps::Strings:    this->stringsStep();    break;
        case DisassemblerEngineSteps::Algorithm:  this->algorithmStep();  break;
        case DisassemblerEngineSteps::Unexplored: this->unexploredStep(); break;
        case DisassemblerEngineSteps::Analyze:    this->analyzeStep();    break;
        case DisassemblerEngineSteps::CFG:        this->cfgStep();        break;
        case DisassemblerEngineSteps::Signature:  this->signatureStep();  break;
        default:                                  r_ctx->log("Unknown step: " + String::number(static_cast<size_t>(step))); return;
    }
}

void DisassemblerEngine::enqueue(address_t address) { m_algorithm->enqueue(address); }
CachedInstruction DisassemblerEngine::decodeInstruction(address_t address) { return m_algorithm->decodeInstruction(address); }

bool DisassemblerEngine::needsWeak() const
{
    switch(m_currentstep)
    {
        case DisassemblerEngineSteps::Strings:
        case DisassemblerEngineSteps::Algorithm:
        case DisassemblerEngineSteps::Unexplored:
            return true;

        default: break;
    }

    return false;
}

bool DisassemblerEngine::busy() const { return m_busy; }

void DisassemblerEngine::stop()
{
    if(!r_ctx->sync()) JobManager::deinitialize();
    this->notify(false);
}

void DisassemblerEngine::stringsStep()
{
    m_starttime = std::chrono::steady_clock::now();
    this->notify(true);

    if(r_ctx->sync())
    {
        for(size_t i = 0; i < r_doc->segmentsCount(); i++)
            this->stringsJob({i, 0});

        this->execute();
    }
    else
        JobManager::dispatch(r_doc->segmentsCount(), this, &DisassemblerEngine::stringsJob);
}

void DisassemblerEngine::algorithmStep()
{
    m_signatures = r_ldr->signatures(); // Preload signatures

    if(r_ctx->sync()) this->algorithmJobSync();
    else JobManager::dispatch(this, &DisassemblerEngine::algorithmJob);
}

void DisassemblerEngine::analyzeStep()
{
    if(r_ctx->hasFlag(ContextFlags::DisableAnalyzer))
    {
        this->execute();
        return;
    }

    if(r_ctx->sync()) this->analyzeJob();
    else JobManager::execute(this, &DisassemblerEngine::analyzeJob);
}

void DisassemblerEngine::unexploredStep()
{
    if(r_ctx->hasFlag(ContextFlags::DisableUnexplored))
    {
        this->execute();
        return;
    }

    this->execute();
}

void DisassemblerEngine::signatureStep()
{
    if(m_signatures.empty() || r_ctx->hasFlag(ContextFlags::DisableSignature))
    {
        this->execute();
        return;
    }

    size_t jobcount = 0, groupsize = 0;
    this->calculateFunctionsThreads(&jobcount, &groupsize);

    String signame = *m_signatures.begin();
    m_signatures.erase(signame);

    r_ctx->status("Matching Signature " + signame.quoted());
    m_sigscanner.load(signame);
    JobManager::dispatch(jobcount, groupsize, this, &DisassemblerEngine::signatureJob);
}

void DisassemblerEngine::cfgStep()
{
    if(r_ctx->hasFlag(ContextFlags::DisableCFG))
    {
        this->execute();
        return;
    }

    r_doc->invalidateGraphs();
    r_ctx->status("Generating CFG...");

    for(size_t i = 0; i < r_doc->segmentsCount(); i++)
        r_doc->segmentCoverageAt(i, REDasm::npos);

    if(r_ctx->sync())
    {
        this->cfgJobSync();
        return;
    }

    size_t jobcount = 0, groupsize = 0;

    if(!this->calculateFunctionsThreads(&jobcount, &groupsize))
    {
        this->execute();
        return;
    }

    JobManager::dispatch(jobcount, groupsize, this, &DisassemblerEngine::cfgJob);
}

void DisassemblerEngine::algorithmJob(const JobDispatchArgs&)
{
    Utils::yloop([&]() -> bool {
                     if(!JobManager::initialized() || !m_algorithm->hasNext()) return false;
                     m_algorithm->next();
                     return true;
                 });

    if(JobManager::last())
        this->execute();
}

void DisassemblerEngine::analyzeJob()
{
    if(m_analyzer)
    {
        r_ctx->status("Analyzing (Fast)...");
        m_analyzer->analyzeFast();
    }
    else
    {
        r_ctx->status("Analyzing...");
        m_analyzer = r_ldr->analyzer();
        m_analyzer->analyze();

        auto duration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - m_starttime);
        if(duration.count()) r_ctx->log("Analysis completed in ~" + String::number(duration.count()) + " second(s)");
        else r_ctx->log("Analysis completed");

        if(m_algorithm->hasNext())
        {
            this->execute(DisassemblerEngineSteps::Algorithm); // Repeat algorithm
            return;
        }
    }

    this->execute();
}

void DisassemblerEngine::signatureJob(const JobDispatchArgs& args)
{
    m_sigscanner.scan(r_doc->functionAt(args.jobIndex));
    if(!JobManager::last()) return;

    size_t c = m_sigscanner.count();
    if(c) r_ctx->log("Found " + String::number(c) + " signature(s)");
    else r_ctx->log("No signatures found");
    this->execute(DisassemblerEngineSteps::Signature); // Repeat...
}

void DisassemblerEngine::cfgJob(const JobDispatchArgs& args)
{
    if(!JobManager::initialized()) return;
    if(args.jobIndex >= r_doc->functionsCount()) return;

    address_t address = r_doc->functionAt(args.jobIndex);
    r_ctx->status("Computing basic blocks @ " + String::hex(address));
    auto g = std::make_unique<FunctionGraph>();
    bool cfgdone = false;

    { // Build CFG
        auto lock = s_lock_safe_ptr(r_doc);
        cfgdone = g->build(address);
    }

    if(cfgdone) // Apply CFG
    {
        auto lock = x_lock_safe_ptr(r_doc);
        lock->segmentCoverage(address, g->bytesCount());

        const NodeList& nodes = g->nodes();

        // Add basic block separators to listing
        for(size_t i = 0; (nodes.size() > 1) && (i < nodes.size() - 1); i++)
        {
            const FunctionBasicBlock* fbb = variant_object<FunctionBasicBlock>(g->data(nodes.at(i)));
            if(!fbb) continue;

            ListingItem item = fbb->endItem();
            if(item.isValid()) lock->separator(item.address);
        }

        lock->graph(address, g.release());
    }

    if(JobManager::last())
        this->execute();
}

void DisassemblerEngine::stringsJobSync()
{
    for(size_t i = 0; i < r_doc->segmentsCount(); i++)
        this->searchStringsAt(i);

    this->execute();
}

void DisassemblerEngine::algorithmJobSync()
{
    while(m_algorithm->hasNext())
        m_algorithm->next();

    this->execute();
}

void DisassemblerEngine::cfgJobSync()
{
    for(size_t i = 0; i < r_doc->functionsCount(); i++)
        this->cfgJob({i, 0});

    this->execute();
}

void DisassemblerEngine::loadSignature(const String& signame)
{
    m_signatures.insert(signame);
    this->execute(DisassemblerEngineSteps::Signature);
}

void DisassemblerEngine::searchStringsAt(size_t index) const
{
    if(index >= r_doc->segmentsCount()) return;

    const Segment* segment = r_doc->segmentAt(index);
    if(!segment->is(SegmentType::Data) || segment->is(SegmentType::Bss)) return;

    StringFinder sf(segment);
    sf.find();
}

bool DisassemblerEngine::calculateFunctionsThreads(size_t* jobcount, size_t* groupsize) const
{
    if(!r_doc->functionsCount())
        return false;

    *jobcount = std::min(JobManager::concurrency(), r_doc->functionsCount());
    *groupsize = std::min<size_t>(1, std::ceil(r_doc->functionsCount() / *jobcount));
    return true;
}

void DisassemblerEngine::notify(bool busy)
{
    m_busy = busy;
    EventManager::trigger(StandardEvents::Disassembler_BusyChanged);
}

void DisassemblerEngine::stringsJob(const JobDispatchArgs& args)
{
    this->searchStringsAt(args.jobIndex);

    if(JobManager::last())
        this->execute();
}

} // namespace REDasm
