#pragma once

#include <rdapi/plugin/analyzer.h>
#include "entry.h"

class Analyzer: public Entry<RDEntryAnalyzer>
{
    public:
        Analyzer(const RDEntryAnalyzer* entry, Context* ctx);
        bool isEnabled() const;
        void execute() const;
};
