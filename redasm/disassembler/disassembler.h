#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include "../plugins/plugins.h"
#include "../support/concurrenttimer.h"
#include "listing/listingdocument.h"
#include "disassemblerbase.h"

namespace REDasm {

class Disassembler: public DisassemblerBase
{
    public:
        Disassembler(AssemblerPlugin* assembler, FormatPlugin* format);
        virtual ~Disassembler();
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
        void disassembleStep(safe_ptr<AssemblerAlgorithm> &algorithm);

    private:
        std::unique_ptr<AssemblerPlugin> m_assembler;
        safe_ptr<AssemblerAlgorithm> m_algorithm;
        ConcurrentTimer m_cctimer;
};

}

#endif // DISASSEMBLER_H
