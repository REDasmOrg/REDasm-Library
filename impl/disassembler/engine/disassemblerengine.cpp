#include "disassemblerengine.h"
#include <redasm/disassembler/disassembler.h>
#include <redasm/plugins/assembler/assembler.h>
#include <redasm/context.h>
#include "stringfinder.h"

namespace REDasm {

DisassemblerEngine::DisassemblerEngine()
{
    m_algorithm = r_asm->createAlgorithm();
    m_stringsjob.setOneShot(true);
    m_analyzejob.setOneShot(true);
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
bool DisassemblerEngine::busy() const { return m_jobs.active(); }
void DisassemblerEngine::stop() { m_jobs.stop(); }
void DisassemblerEngine::pause() { m_jobs.pause(); }
void DisassemblerEngine::resume() { m_jobs.resume(); }

void DisassemblerEngine::stringsStep() { m_stringsjob.work(std::bind(&DisassemblerEngine::stringsJob, this, std::placeholders::_1)); }
void DisassemblerEngine::algorithmStep() { m_jobs.work(std::bind(&DisassemblerEngine::algorithmJob, this, std::placeholders::_1)); }
void DisassemblerEngine::analyzeStep() { m_analyzejob.work(std::bind(&DisassemblerEngine::analyzeJob, this, std::placeholders::_1)); }

void DisassemblerEngine::unexploredStep()
{
    stepCompleted();
    this->execute();
}

void DisassemblerEngine::signatureStep()
{

}

void DisassemblerEngine::cfgStep()
{

}

void DisassemblerEngine::algorithmJob(Job *job)
{
    if(m_algorithm->hasNext())
    {
        m_algorithm->next();
        return;
    }

    job->stop();

    if(m_jobs.active())
        return;

    stepCompleted();
    this->execute();
}

void DisassemblerEngine::analyzeJob(Job*)
{
    m_algorithm->analyze();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - m_starttime);

    if(duration.count()) r_ctx->log("Analysis completed in ~" + String::number(duration.count()) + " second(s)");
    else r_ctx->log("Analysis completed");

    stepCompleted();
    this->execute();
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
