#pragma once

#include "../../analyzer/analyzer.h"

namespace REDasm {

class N64Analyzer : public Analyzer
{
    public:
        N64Analyzer(DisassemblerAPI* disassembler);
        virtual void analyze();       
};

} // namespace REDasm
