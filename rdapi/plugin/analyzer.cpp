#include "analyzer.h"
#include <rdcore/plugin/interface/pluginmodule.h>
#include <rdcore/plugin/analyzer.h>

bool RDAnalyzer_Register(RDPluginModule* pm, const RDEntryAnalyzer* entry) { return CPTR(PluginModule, pm)->registerEntry(entry); }
bool RDAnalyzer_IsExperimental(const RDAnalyzer* analyzer) { return HAS_FLAG(CPTR(const Analyzer, analyzer)->plugin(), AnalyzerFlags_Experimental); }
const char* RDAnalyzer_GetDescription(const RDAnalyzer* analyzer) { return CPTR(const Analyzer, analyzer)->plugin()->description; }
const char* RDAnalyzer_GetName(const RDAnalyzer* analyzer) { return CPTR(const Analyzer, analyzer)->plugin()->name; }
const char* RDAnalyzer_GetId(const RDAnalyzer* analyzer) { return CPTR(const Analyzer, analyzer)->plugin()->id; }
u32 RDAnalyzer_GetOrder(const RDAnalyzer* analyzer) { return CPTR(const Analyzer, analyzer)->plugin()->order; }
