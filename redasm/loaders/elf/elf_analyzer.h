#pragma once

#include "../../analyzer/analyzer.h"

namespace REDasm {

class ElfAnalyzer: public Analyzer
{
    public:
        ElfAnalyzer(DisassemblerAPI* disassembler);
        void analyze() override;

    private:
        void findMain_x86(const Symbol* symlibcmain);
        void findMain_x86_64(ListingDocumentType::const_iterator it);
        void findMain_x86(ListingDocumentType::const_iterator it);

   private:
        void disassembleLibStartMain();
        Symbol *getLibStartMain();

   protected:
        std::unordered_map<std::string, address_t> m_libcmain;
};

} // namespace REDasm
