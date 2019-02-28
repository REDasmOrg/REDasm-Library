#include "pe_analyzer.h"
#include "pe_utils.h"
#include "pe_constants.h"
#include "../../support/rtti/msvc/rtti_msvc.h"

#define IMPORT_NAME(library, name) PEUtils::importName(library, name)
#define IMPORT_TRAMPOLINE(library, name) ("_" + IMPORT_NAME(library, name))
#define ADD_WNDPROC_API(argidx, name) m_wndprocapi.emplace_front(argidx, name)

namespace REDasm {

PEAnalyzer::PEAnalyzer(u32 petype, DisassemblerAPI *disassembler, const SignatureFiles& signatures): Analyzer(disassembler, signatures), m_petype(petype)
{
    ADD_WNDPROC_API(4, "DialogBoxA");
    ADD_WNDPROC_API(4, "DialogBoxW");
    ADD_WNDPROC_API(4, "DialogBoxParamA");
    ADD_WNDPROC_API(4, "DialogBoxParamW");
    ADD_WNDPROC_API(4, "CreateDialogParamA");
    ADD_WNDPROC_API(4, "CreateDialogParamW");
    ADD_WNDPROC_API(4, "CreateDialogIndirectParamA");
    ADD_WNDPROC_API(4, "CreateDialogIndirectParamW");
}

void PEAnalyzer::analyze()
{
    Analyzer::analyze();

    if(m_petype <= PeType::Msvc)
        this->findCRTWinMain();

    if(m_petype == PeType::Msvc)
    {
        this->findAllWndProc();

        REDasm::log("MSVC Compiler detected, searching RTTI...");

        if(m_disassembler->format()->bits() == 64)
            RTTI::RTTIMsvc<u64>(m_disassembler).search();
        else
            RTTI::RTTIMsvc<u32>(m_disassembler).search();

        return;
    }

    if(m_petype == PeType::None)
        REDasm::log("WARNING: Cannot detect PE Type");

    this->findAllWndProc();
}

Symbol* PEAnalyzer::getImport(const std::string &library, const std::string &api)
{
    Symbol* symbol = m_disassembler->document()->symbol(IMPORT_TRAMPOLINE(library, api));

    if(!symbol)
        symbol = m_disassembler->document()->symbol(IMPORT_NAME(library, api));

    return symbol;
}

ReferenceVector PEAnalyzer::getAPIReferences(const std::string &library, const std::string &api)
{
    Symbol* symbol = this->getImport(library, api);

    if(!symbol)
        return ReferenceVector();

    return m_disassembler->getReferences(symbol->address);
}

void PEAnalyzer::findAllWndProc()
{
    for(auto it = m_wndprocapi.begin(); it != m_wndprocapi.end(); it++)
    {
        ReferenceVector refs = this->getAPIReferences("user32.dll", it->second);

        for(address_t ref : refs)
            this->findWndProc(ref, it->first);
    }
}

void PEAnalyzer::findWndProc(address_t address, size_t argidx)
{
    auto it = m_document->instructionItem(address);

    if(it == m_document->end())
        return;

    size_t arg = 0;
    it--; // Skip call

    while(arg < argidx)
    {
        const InstructionPtr& instruction = m_document->instruction((*it)->address);

        if(instruction->is(InstructionTypes::Push))
        {
            arg++;

            if(arg == argidx)
            {
                const Operand* op = instruction->op(0);
                Segment* segment = m_document->segment(op->u_value);

                if(segment && segment->is(SegmentTypes::Code))
                {
                    m_document->lockFunction(op->u_value, "DlgProc_" + REDasm::hex(op->u_value));
                    m_disassembler->disassemble(op->u_value);
                }
            }
        }

        if((arg == argidx) || (it == m_document->begin()) || instruction->is(InstructionTypes::Stop))
            break;

        it--;
    }
}

void PEAnalyzer::findCRTWinMain()
{
    InstructionPtr instruction = m_document->entryInstruction(); // Look for call

    if(!instruction || !instruction->is(InstructionTypes::Call))
        return;

    Symbol* symbol = m_document->symbol(PE_SECURITY_COOKIE_SYMBOL);

    if(!symbol)
        return;

    bool found = false;
    ReferenceVector refs = m_disassembler->getReferences(symbol->address);

    for(address_t ref : refs)
    {
        ListingItem* scfuncitem = m_document->functionStart(ref);

        if(!scfuncitem || ((instruction->target() != scfuncitem->address)))
            continue;

        m_document->lock(scfuncitem->address, "__security_init_cookie");
        found = true;
        break;
    }

    if(!found || !m_document->advance(instruction) || !instruction->is(InstructionTypes::Jump))
        return;

    m_document->lock(instruction->target(), "__mainCRTStartup", SymbolTypes::Function);
    m_document->setDocumentEntry(instruction->target());
}

}
