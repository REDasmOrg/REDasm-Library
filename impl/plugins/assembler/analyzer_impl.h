#pragma once

#include <redasm/disassembler/types/symboltable.h>
#include <redasm/disassembler/disassembler.h>
#include <redasm/plugins/loader/analyzer.h>
#include <redasm/support/dispatcher.h>

namespace REDasm {

class AnalyzerImpl
{
    PIMPL_DECLARE_Q(Analyzer)
    PIMPL_DECLARE_PUBLIC(Analyzer)

    public:
        AnalyzerImpl(Disassembler* disassembler);
        ListingDocument& document() const;
        Disassembler* disassembler() const;
        bool findNullSubs(const Symbol* symbol);
        void findTrampoline(const Symbol* symbol);
        void checkFunctions();
        void loadSignatures();
        void analyzeFast();

    protected:
        Dispatcher<String, void*> m_archdispatcher;
        ListingDocument& m_document;
        Disassembler* m_disassembler;
};

} // namespace REDasm
