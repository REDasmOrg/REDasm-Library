#pragma once

#include "../plugins/plugins.h"
#include "../support/concurrent/jobspool.h"
#include "listing/listingdocument.h"
#include "disassemblerbase.h"
#include <chrono>

namespace REDasm {

class Disassembler: public DisassemblerBase
{
    public:
        Disassembler(AssemblerPlugin* assembler, LoaderPlugin* loader);
        virtual ~Disassembler() = default;
        void disassemble() override;

    public: // Primitive functions
        Printer* createPrinter() override;
        InstructionPtr disassembleInstruction(address_t address) override;
        void disassemble(address_t address) override;
        void stop() override;
        void pause() override;
        void resume() override;
        size_t state() const override;
        bool busy() const override;

    private:
        void work();
        void disassembleJob();
        void disassembleStep(Job *job);
        void analyzeStep();

    private:
        std::chrono::steady_clock::time_point m_starttime;
        safe_ptr<AssemblerAlgorithm> m_algorithm;
        Job m_analyzejob;
        JobsPool m_jobs;
};

}
