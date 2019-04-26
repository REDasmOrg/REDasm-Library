#pragma once

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
