#include "psxexe_analyzer.h"
#include "../../plugins/loader.h"

namespace REDasm {

PsxExeAnalyzer::PsxExeAnalyzer(DisassemblerAPI *disassembler): Analyzer(disassembler) { }

void PsxExeAnalyzer::analyze()
{
    Analyzer::analyze();
    this->detectMain();
}

void PsxExeAnalyzer::detectMain()
{
    const Symbol* symep = m_document->documentEntry();

    if(!symep)
        return;

    bool initheap = false;
    auto it = m_document->instructionItem(symep->address);

    for(; it != m_document->end(); it++)
    {
        if((*it)->is(ListingItem::SymbolItem))
            continue;

        if(!(*it)->is(ListingItem::InstructionItem))
            break;

        InstructionPtr instruction = m_document->instruction((*it)->address);

        if(instruction->id != MIPS_INS_JAL)
            continue;

        Symbol* symbol = m_document->symbol(instruction->target());

        if(!symbol)
            continue; // Continue until InitHeap is found

        if(initheap)
        {
            m_document->lock(symbol->address, "main");
            m_document->setDocumentEntry(symbol->address);
            REDasm::log("Found 'main' @ " + REDasm::hex(symbol->address));
            break;
        }

        if(symbol->name == "LIBAPI.InitHeap")
            initheap = true;
    }
}

}
