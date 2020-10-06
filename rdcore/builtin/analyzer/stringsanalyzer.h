#pragma once

#include <rdapi/types.h>
#include <rdapi/plugin/analyzer.h>

class Context;

extern RDEntryAnalyzer analyzerEntry_Strings;

class StringsAnalyzer
{
    public:
        StringsAnalyzer() = delete;
        static void analyze(Context* ctx);
};
