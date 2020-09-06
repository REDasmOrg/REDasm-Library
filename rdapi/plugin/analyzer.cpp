#include "analyzer.h"
#include <rdcore/context.h>

void RD_GetEnabledAnalyzers(Callback_AnalyzerPlugin callback, void* userdata) { rd_ctx->getEnabledAnalyzers(callback, userdata);  }
void RDAnalyzer_Select(const RDAnalyzerPlugin* panalyzer, bool selected) { rd_ctx->selectAnalyzer(const_cast<RDAnalyzerPlugin*>(panalyzer), selected); }
bool RDAnalyzer_Register(RDAnalyzerPlugin* plugin) { return rd_ctx->registerPlugin(plugin); }
