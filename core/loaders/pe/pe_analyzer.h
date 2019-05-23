﻿#pragma once

#include "../../analyzer/analyzer.h"
#include "pe_classifier.h"
#include <forward_list>

namespace REDasm {

class PEAnalyzer: public Analyzer
{
    private:
        typedef std::pair<size_t, std::string> APIInfo;

    public:
        PEAnalyzer(const PEClassifier* classifier, DisassemblerAPI* disassembler);
        void analyze() override;

    private:
        Symbol *getImport(const std::string& library, const std::string& api);
        ReferenceVector getAPIReferences(const std::string& library, const std::string& api);
        void findWndProc(address_t address, size_t argidx);
        void findCRTWinMain();
        void findAllWndProc();

    protected:
        const PEClassifier* m_classifier;

    private:
        std::forward_list<APIInfo> m_wndprocapi;
};

} // namespace REDasm
