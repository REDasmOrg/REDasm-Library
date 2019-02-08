#ifndef ANALYZER_H
#define ANALYZER_H

#include <functional>
#include <memory>
#include "../support/dispatcher.h"
#include "../plugins/assembler/assembler.h"
#include "../disassembler/listing/listingdocument.h"
#include "../disassembler/types/symboltable.h"
#include "../disassembler/disassemblerapi.h"
#include "../database/signaturedb.h"

namespace REDasm {

class Analyzer
{
    public:
        Analyzer(DisassemblerAPI* disassembler, const SignatureFiles& signaturefiles);
        virtual ~Analyzer();
        virtual void analyzeFast();
        virtual void analyze();

    private:
        void loadSignatures();
        bool findNullSubs(const SymbolPtr &symbol);
        void checkFunctions();
        void findTrampoline(const SymbolPtr &symbol);
        SymbolPtr findTrampoline_x86(ListingDocumentType::iterator it);
        SymbolPtr findTrampoline_arm(ListingDocumentType::iterator it);

    protected:
        ListingDocument& m_document;
        DisassemblerAPI* m_disassembler;
        Dispatcher<std::string, void*> m_archdispatcher;
        const SignatureFiles& m_signaturefiles;
};

}

#endif // ANALYZER_H
