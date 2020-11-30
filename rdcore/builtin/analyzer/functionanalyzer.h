#pragma once

#include <rdapi/types.h>
#include <rdapi/plugin/analyzer.h>
#include <string>

class Context;
class ILExpression;

extern RDEntryAnalyzer analyzerEntry_Function;

class FunctionAnalyzer
{
    public:
        FunctionAnalyzer() = delete;
        static void analyze(Context* ctx);

    private:
        static bool findNullSubs(Context* ctx, const ILExpression* expr, rd_address address);
        static std::string findThunk(Context* ctx, const ILExpression* expr, rd_address address, int level = 1);
};

