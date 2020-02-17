#pragma once

#include <redasm/plugins/assembler/algorithm/algorithm.h>
#include <redasm/plugins/loader/analyzer.h>
#include <redasm/plugins/loader/loader.h>
#include <redasm/disassembler/listing/document/listingdocument.h>
#include <redasm/support/jobmanager.h>
#include <redasm/support/safe_ptr.h>
#include <chrono>
#include "signaturescanner.h"

namespace REDasm {

namespace DisassemblerEngineSteps {

enum {
    None,
    Strings, Algorithm, Unexplored, Analyze, CFG, Signature,
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
        void loadSignature(const String& signame);
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
        void cfgStep();
        void signatureStep();

    private: // Threaded Variants
        void stringsJob(const JobDispatchArgs& args);
        void algorithmJob(const JobDispatchArgs&);
        void analyzeJob();
        void cfgJob(const JobDispatchArgs& args);
        void signatureJob(const JobDispatchArgs& args);

    private: // Sync Variants
        void stringsJobSync();
        void algorithmJobSync();
        void cfgJobSync();

    private:
        void searchStringsAt(size_t index) const;
        void notify(bool busy);

    private:
        bool m_busy{false};
        Analyzer* m_analyzer{nullptr};
        SignatureScanner m_sigscanner;
        SignatureIdentifiers m_signatures;
        std::chrono::steady_clock::time_point m_starttime;
        size_t m_currentstep{DisassemblerEngineSteps::None};
        safe_ptr<Algorithm> m_algorithm;
};

} // namespace REDasm
