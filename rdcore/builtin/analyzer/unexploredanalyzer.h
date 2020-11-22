#pragma once

#include <rdapi/types.h>
#include <rdapi/plugin/analyzer.h>
#include <unordered_set>

class Context;

extern RDEntryAnalyzer analyzerEntry_Unexplored;

class UnexploredAnalyzer
{
    public:
        UnexploredAnalyzer() = delete;
        static void analyze(Context* ctx);
        static void clearDone();

    private:
        static std::unordered_set<rd_address> m_done;
};

