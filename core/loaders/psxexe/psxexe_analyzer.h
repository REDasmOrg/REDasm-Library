#pragma once

#include "../../analyzer/analyzer.h"

namespace REDasm {

class PsxExeAnalyzer: public Analyzer
{
    public:
        PsxExeAnalyzer(DisassemblerAPI* disassembler);
        void analyze() override;

    private:
        void detectMain();
};

}
