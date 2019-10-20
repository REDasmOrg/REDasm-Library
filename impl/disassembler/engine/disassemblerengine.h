#pragma once

#include <redasm/plugins/assembler/algorithm/algorithm.h>
#include <redasm/plugins/loader/analyzer.h>
#include <redasm/disassembler/listing/listingdocumentnew.h>
#include <redasm/disassembler/concurrent/jobspool.h>
#include <redasm/support/safe_ptr.h>
#include <redasm/support/event.h>
#include <chrono>

namespace REDasm {

namespace DisassemblerEngineSteps {

enum {
    None,
    Strings, Algorithm, Unexplored, Analyze, Signature, CFG,
    Last = CFG
};

} // namespace DisassembleEngineSteps

class DisassemblerEngine
{
    public:
        Event stepCompleted;

    public:
        DisassemblerEngine();
        size_t currentStep() const;
        size_t concurrency() const;
        void reset();
        void init();
        void execute();
        void execute(size_t step);
        void enqueue(address_t address);

    public:
        JobState state() const;
        bool needsWeak() const;
        bool busy() const;
        void stop();
        void pause();
        void resume();

    private:
        void stringsStep();
        void algorithmStep();
        void analyzeStep();
        void unexploredStep();
        void signatureStep();
        void cfgStep();

    private:
        void stringsJob(Job*);
        void algorithmJob(Job* job);
        void analyzeJob(Job*);
        void cfgJob(Job*);

    private:
        void cfg(document_x_lock_new& lock, address_t address);

    private:
        Analyzer* m_analyzer{nullptr};
        std::chrono::steady_clock::time_point m_starttime;
        size_t m_currentstep{DisassemblerEngineSteps::None};
        safe_ptr<Algorithm> m_algorithm;
        Job m_stringsjob, m_analyzejob, m_cfgjob;
        JobsPool m_jobs;
};

} // namespace REDasm
