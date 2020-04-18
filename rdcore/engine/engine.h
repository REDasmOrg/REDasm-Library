#pragma once

#include <chrono>
#include <atomic>
#include <rdapi/types.h>
#include "algorithm/algorithm.h"
#include "../support/job/jobmanager.h"
#include "../support/safe_ptr.h"

class Disassembler;
class Analyzer;

class Engine
{
    public:
        enum { EngineState_None, EngineState_Strings, EngineState_Algorithm, EngineState_Unexplored,
               EngineState_Analyze, EngineState_CFG, EngineState_Signature, EngineState_Last };

    private:
        typedef safe_ptr<Algorithm> SafeAlgorithm;

    public:
        Engine(Disassembler* disassembler);
        ~Engine();
        size_t currentStep() const;
        size_t concurrency() const;
        void reset();
        void execute();
        void execute(size_t step);
        void enqueue(address_t address);

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
        std::atomic<size_t> m_sigcount{0}, m_siganalyzed{0};
        std::unique_ptr<Analyzer> m_analyzer;
        //SignatureScanner m_sigscanner;
        //SignatureIdentifiers m_signatures;
        std::chrono::steady_clock::time_point m_starttime;
        size_t m_currentstep{Engine::EngineState_None};
        Disassembler* m_disassembler;
        SafeAlgorithm m_algorithm;
};

