#pragma once

#include "../../pimpl.h"
#include "../../macros.h"

namespace REDasm {

class AnalyzerImpl;
class Disassembler;

class LIBREDASM_API Analyzer
{
    PIMPL_DECLARE_P(Analyzer)
    PIMPL_DECLARE_PRIVATE(Analyzer)

    public:
        Analyzer(Disassembler* disassembler);
        virtual ~Analyzer() = default;
        virtual void analyzeFast();
        virtual void analyze();
};

}
