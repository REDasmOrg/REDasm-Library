#include "disassemblerengine.h"
#include <redasm/disassembler/disassembler.h>
#include <redasm/plugins/assembler/assembler.h>
#include <redasm/graph/functiongraph.h>
#include <redasm/context.h>
#include "stringfinder.h"

namespace REDasm {

DisassemblerEngine::DisassemblerEngine()
{
    m_algorithm = r_asm->createAlgorithm();
    m_stringsjob.setOneShot(true);
    m_analyzejob.setOneShot(true);
    m_cfgjob.setOneShot(true);
}

size_t DisassemblerEngine::currentStep() const { return m_currentstep; }
size_t DisassemblerEngine::concurrency() const { return m_jobs.concurrency(); }
void DisassemblerEngine::reset() { m_currentstep = DisassemblerEngineSteps::None; }
void DisassemblerEngine::execute() { if(m_currentstep < DisassemblerEngineSteps::Last) this->execute(++m_currentstep); }

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
        case DisassemblerEngineSteps::Signature:  this->signatureStep();  break;
        case DisassemblerEngineSteps::CFG:        this->cfgStep();        break;
        default:                                  r_ctx->log("Unknown step: " + String::number(static_cast<size_t>(step))); return;
    }
}

void DisassemblerEngine::enqueue(address_t address) { m_algorithm->enqueue(address); }
JobState DisassemblerEngine::state() const { return m_jobs.state(); }

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

bool DisassemblerEngine::busy() const { return m_jobs.active(); }
void DisassemblerEngine::stop() { m_jobs.stop(); }
void DisassemblerEngine::pause() { m_jobs.pause(); }
void DisassemblerEngine::resume() { m_jobs.resume(); }
void DisassemblerEngine::stringsStep() { m_stringsjob.work(std::bind(&DisassemblerEngine::stringsJob, this, std::placeholders::_1)); }
void DisassemblerEngine::algorithmStep() { m_jobs.work(std::bind(&DisassemblerEngine::algorithmJob, this, std::placeholders::_1)); }
void DisassemblerEngine::analyzeStep() { m_analyzejob.work(std::bind(&DisassemblerEngine::analyzeJob, this, std::placeholders::_1)); }

void DisassemblerEngine::unexploredStep()
{
    r_docnew->moveToEntry();

    stepCompleted();
    this->execute();
}

void DisassemblerEngine::signatureStep()
{
    stepCompleted();
    this->execute();
}

void DisassemblerEngine::cfgStep() { m_cfgjob.work(std::bind(&DisassemblerEngine::cfgJob, this, std::placeholders::_1)); }

void DisassemblerEngine::algorithmJob(Job *job)
{
    if(m_algorithm->hasNext())
    {
        m_algorithm->next();
        return;
    }

    job->stop();
    if(m_jobs.active()) return;

    stepCompleted();
    this->execute();
}

void DisassemblerEngine::analyzeJob(Job*)
{
    if(m_analyzer)
    {
        r_ctx->status("Analyzing (Fast)...");
        m_analyzer->analyzeFast();
    }
    else
    {
        m_analyzer = r_ldr->analyzer();
        r_ctx->status("Analyzing...");
        m_analyzer->analyze();

        // Trigger a Fast Analysis when post disassembling is completed
        r_disasm->busyChanged.connect(this, [&](EventArgs*) {
            //if(!r_disasm->busy()) this->execute(DisassemblerEngineSteps::Analyze);
        });

        auto duration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - m_starttime);
        if(duration.count()) r_ctx->log("Analysis completed in ~" + String::number(duration.count()) + " second(s)");
        else r_ctx->log("Analysis completed");
    }

    r_docnew->moveToEntry();
    stepCompleted();
    this->execute();
}

void DisassemblerEngine::cfgJob(Job*)
{
    r_ctx->log("Generating CFG...");
    auto lock = x_lock_safe_ptr(r_docnew);
    lock->invalidateGraphs();

    for(size_t i = 0; i < r_docnew->segments()->size(); i++)
        r_docnew->segmentCoverageAt(i, REDasm::npos);

    const ListingFunctions* lf = lock->functions();

    for(size_t i = 0; i < lf->size(); i++)
        this->cfg(lock, lf->at(i));

    stepCompleted();
    this->execute();
}

void DisassemblerEngine::cfg(document_x_lock_new &lock, address_t address)
{
    r_ctx->status("Computing basic blocks @ " + String::hex(address));
    auto g = std::make_unique<FunctionGraph>();
    if(!g->build(address)) return;

    r_docnew->segmentCoverage(address, g->bytesCount());
    const NodeList& nodes = g->nodes();

    // Add basic block separators to listing
    // for(size_t i = 0; (nodes.size() > 1) && (i < nodes.size() - 1); i++)
    // {
    //     const FunctionBasicBlock* fbb = variant_object<FunctionBasicBlock>(g->data(nodes.at(i)));
    //     if(!fbb) continue;

    //     const ListingItem* item = fbb->instructionEndItem();
    //     if(!item) continue;

    //     lock->separator(item->address_new);
    // }

    lock->graph(address, g.release());
}

void DisassemblerEngine::stringsJob(Job*)
{
    m_starttime = std::chrono::steady_clock::now();
    auto* segments = r_docnew->segments();

    for(size_t i = 0; i < segments->size(); i++)
    {
        const Segment* segment = segments->at(i);
        if(!segment->is(SegmentType::Data) || segment->is(SegmentType::Bss)) continue;

        r_ctx->status("Searching strings @ " + segment->name.quoted());
        StringFinder sf(r_ldr->viewSegment(segment));
        sf.find();
    }

    stepCompleted();
    this->execute();
}

} // namespace REDasm
