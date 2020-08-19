#pragma once

#include <rdapi/types.h>
#include <rdapi/plugin/analyzer.h>

class Disassembler;
class ILFunction;

extern RDAnalyzerPlugin analyzer_Function;

class FunctionAnalyzer
{
    public:
        FunctionAnalyzer() = delete;
        static void analyze(Disassembler* disassembler);

    private:
        static bool findNullSubs(Disassembler* disassembler, const ILFunction* il, rd_address address);
        static void findThunk(Disassembler* disassembler, const ILFunction* il, rd_address address);
};

