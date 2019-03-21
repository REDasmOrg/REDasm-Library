#ifndef N64_ANALYZER_H
#define N64_ANALYZER_H

#include "../../analyzer/analyzer.h"

namespace REDasm {

class N64Analyzer : public Analyzer
{
    public:
        N64Analyzer(DisassemblerAPI* disassembler);
        virtual void analyze();       
};

} // namespace REDasm

#endif // N64_ANALYZER_H
