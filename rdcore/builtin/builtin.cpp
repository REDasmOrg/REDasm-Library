#include "builtin.h"
#include "loader/binary.h"
#include "analyzer/functionanalyzer.h"
#include "analyzer/stringsanalyzer.h"
#include "analyzer/unexploredanalyzer.h"
#include "../plugin/interface/category.h"

#define BUILTIN_ENTRY(x) reinterpret_cast<const RDEntry*>(x)

Builtins::Builtins()
{
    // Loaders
    entries.insert({ EntryCategory_Loader, BUILTIN_ENTRY(&loaderEntry_Binary) });

    // Analyzers
    entries.insert({ EntryCategory_Analyzer, BUILTIN_ENTRY(&analyzerEntry_Function) });
    entries.insert({ EntryCategory_Analyzer, BUILTIN_ENTRY(&analyzerEntry_Strings) });
    entries.insert({ EntryCategory_Analyzer, BUILTIN_ENTRY(&analyzerEntry_Unexplored) });
}

const Builtins BUILTINS;
