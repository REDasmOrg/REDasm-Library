#pragma once

#include <rdapi/types.h>
#include <rdapi/plugin/analyzer.h>

class Context;

extern RDEntryAnalyzer analyzerEntry_Unexplored;

class UnexploredAnalyzer
{
    public:
        UnexploredAnalyzer() = delete;
        static void analyze(Context* ctx);
};

