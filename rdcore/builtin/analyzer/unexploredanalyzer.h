#pragma once

#include <rdapi/types.h>
#include <rdapi/plugin/analyzer.h>

class Disassembler;
class ILFunction;

extern RDAnalyzerPlugin analyzer_Unexplored;

class UnexploredAnalyzer
{
    public:
        UnexploredAnalyzer() = delete;
        static void analyze(Disassembler* disassembler);
};

