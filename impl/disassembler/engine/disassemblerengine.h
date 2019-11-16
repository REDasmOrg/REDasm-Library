#pragma once

#include <redasm/plugins/assembler/algorithm/algorithm.h>
#include <redasm/plugins/loader/analyzer.h>
#include <redasm/disassembler/listing/document/listingdocumentnew.h>
#include <redasm/support/jobmanager.h>
#include <redasm/support/safe_ptr.h>
#include <chrono>

namespace REDasm {

namespace DisassemblerEngineSteps {

enum {
    None,
    Strings, Algorithm, Unexplored, Analyze, Signature, CFG,
    Last
};

} // namespace DisassembleEngineSteps

class DisassemblerEngine
{
    public:
        DisassemblerEngine();
        ~DisassemblerEngine();
        size_t currentStep() const;
        size_t concurrency() const;
        void reset();
        void init();
        void execute();
        void execute(size_t step);
        void enqueue(address_t address);
        CachedInstruction decodeInstruction(address_t address);

    public:
        bool needsWeak() const;
        bool busy() const;
        void stop();

    private:
        void stringsStep();
        void algorithmStep();
        void analyzeStep();
        void unexploredStep();
        void signatureStep();
        void cfgStep();

    private: // Threaded Variants
        void stringsJob(const JobDispatchArgs& args);
        void algorithmJob(const JobDispatchArgs&);
        void analyzeJob();
        void cfgJob(const JobDispatchArgs& args);

    private: // Sync Variants
        void stringsJobSync();
        void algorithmJobSync();
        void cfgJobSync();

    private:
        void searchStringsAt(size_t index) const;
        bool calculateCfgThreads(size_t* jobcount, size_t* groupsize) const;
        void notify(bool busy);

    private:
        bool m_busy{false};
        Analyzer* m_analyzer{nullptr};
        std::chrono::steady_clock::time_point m_starttime;
        size_t m_currentstep{DisassemblerEngineSteps::None};
        safe_ptr<Algorithm> m_algorithm;
};

} // namespace REDasm
