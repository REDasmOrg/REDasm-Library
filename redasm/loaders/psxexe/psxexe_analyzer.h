#ifndef PSXEXE_ANALYZER_H
#define PSXEXE_ANALYZER_H

#include "../../analyzer/analyzer.h"

namespace REDasm {

class PsxExeAnalyzer: public Analyzer
{
    public:
        PsxExeAnalyzer(DisassemblerAPI* disassembler);
        virtual void analyze();

    private:
        void detectMain();
};

}

#endif // PSXEXE_ANALYZER_H
