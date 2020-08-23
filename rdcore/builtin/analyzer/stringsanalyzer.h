#pragma once

#include <rdapi/types.h>
#include <rdapi/plugin/analyzer.h>

class Disassembler;

extern RDAnalyzerPlugin analyzer_Strings;

class StringsAnalyzer
{
    public:
        StringsAnalyzer() = delete;
        static void analyze(Disassembler* disassembler);
};
