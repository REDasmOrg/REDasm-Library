#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

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
        virtual void disassemble();

    public: // Primitive functions
        virtual Printer* createPrinter();
        virtual AssemblerPlugin* assembler();
        virtual InstructionPtr disassembleInstruction(address_t address);
        virtual void disassemble(address_t address);
        virtual void stop();
        virtual void pause();
        virtual void resume();
        virtual size_t state() const;
        virtual bool busy() const;

    private:
        void work();
        void disassembleJob();
        void disassembleStep(Job *job);
        void analyzeStep();

    private:
        std::chrono::steady_clock::time_point m_starttime;
        std::unique_ptr<AssemblerPlugin> m_assembler;
        safe_ptr<AssemblerAlgorithm> m_algorithm;
        Job m_analyzejob;
        JobsPool m_jobs;
};

}

#endif // DISASSEMBLER_H
