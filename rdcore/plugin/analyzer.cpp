#include "analyzer.h"

Analyzer::Analyzer(const RDEntryAnalyzer* entry, Context* ctx): Entry<RDEntryAnalyzer>(entry, ctx) { }

bool Analyzer::isEnabled() const
{
    if(m_entry->isenabled) return m_entry->isenabled(CPTR(const RDContext, this->context()));
    return false;
}

void Analyzer::execute() const { if(m_entry->execute) m_entry->execute(CPTR(RDContext, this->context())); }
