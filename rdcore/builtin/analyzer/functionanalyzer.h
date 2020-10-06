#pragma once

#include <rdapi/types.h>
#include <rdapi/plugin/analyzer.h>

class Context;
class Disassembler;
class ILFunction;

extern RDEntryAnalyzer analyzerEntry_Function;

class FunctionAnalyzer
{
    public:
        FunctionAnalyzer() = delete;
        static void analyze(Context* ctx);

    private:
        static bool findNullSubs(Context* ctx, const ILFunction* il, rd_address address);
        static void findThunk(Context* ctx, const ILFunction* il, rd_address address);
};

