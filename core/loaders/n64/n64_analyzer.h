#pragma once

#include "../../analyzer/analyzer.h"

namespace REDasm {

class N64Analyzer : public Analyzer
{
    public:
        N64Analyzer(DisassemblerAPI* disassembler);
        void analyze() override;
};

} // namespace REDasm
