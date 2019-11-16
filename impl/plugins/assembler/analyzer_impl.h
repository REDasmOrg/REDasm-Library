#pragma once

#include <redasm/disassembler/listing/backend/symbols/symboltable.h>
#include <redasm/plugins/loader/analyzer.h>
#include <redasm/support/dispatcher.h>

namespace REDasm {

class AnalyzerImpl
{
    PIMPL_DECLARE_Q(Analyzer)
    PIMPL_DECLARE_PUBLIC(Analyzer)

    public:
        AnalyzerImpl() = default;
        bool findNullSubs(address_t address);
        void findTrampoline(address_t address);
        void checkFunctions();
        void loadSignatures();
        void analyzeFast();

    protected:
        Dispatcher<String, void*> m_archdispatcher;
};

} // namespace REDasm
