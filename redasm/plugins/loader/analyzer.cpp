#include "analyzer.h"
#include <impl/plugins/assembler/analyzer_impl.h>

namespace REDasm {

Analyzer::Analyzer(): m_pimpl_p(new AnalyzerImpl()) { }
void Analyzer::analyzeFast() { PIMPL_P(Analyzer); p->analyzeFast(); }
void Analyzer::analyze() { this->analyzeFast(); }

} // namespace REDasm
