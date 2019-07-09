#include "analyzer.h"
#include <impl/plugins/assembler/analyzer_impl.h>

namespace REDasm {

Analyzer::Analyzer(Disassembler *disassembler): m_pimpl_p(new AnalyzerImpl(disassembler)) { }
void Analyzer::analyzeFast() { PIMPL_P(Analyzer); p->analyzeFast(); }
void Analyzer::analyze() { this->analyzeFast(); }
ListingDocument &Analyzer::document() const { PIMPL_P(const Analyzer); return p->document(); }
Disassembler *Analyzer::disassembler() const { PIMPL_P(const Analyzer); return p->disassembler(); }

} // namespace REDasm
