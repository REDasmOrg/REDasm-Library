#include "analyzer.h"
#include "../support/hash.h"

namespace REDasm {

Analyzer::Analyzer(DisassemblerAPI *disassembler, const SignatureFiles &signaturefiles): m_disassembler(disassembler), m_signaturefiles(signaturefiles)
{
    m_document = disassembler->document();

    // Fast post analysis
    m_disassembler->busyChanged += [&]() {
        if(m_disassembler->busy())
            return;

        this->findTrampolines();
    };
}

Analyzer::~Analyzer() { }

void Analyzer::analyze()
{
    this->loadSignatures();
    this->findTrampolines();
}

void Analyzer::findTrampolines()
{
    m_disassembler->document()->symbols()->iterate(SymbolTypes::FunctionMask, [this](SymbolPtr symbol) -> bool {
        this->findTrampolines(symbol);
        return true;
    });
}

void Analyzer::loadSignatures()
{
    for(const std::string& sdbfile : m_signaturefiles)
        m_disassembler->loadSignature(REDasm::makeDbPath("sdb", sdbfile));
}

void Analyzer::findTrampolines(SymbolPtr symbol)
{
    if(symbol->is(SymbolTypes::Locked))
        return;

    auto it = m_document->instructionItem(symbol->address);

    if(it == m_document->end())
        return;

    const AssemblerPlugin* assembler = m_disassembler->assembler();
    SymbolPtr symimport;

    if(ASSEMBLER_IS(assembler, "x86"))
        symimport = this->findTrampolines_x86(it);
    else if(ASSEMBLER_IS(assembler, "ARM"))
        symimport = this->findTrampolines_arm(it);

    if(!symimport || !symimport->is(SymbolTypes::Import))
        return;

    m_document->lock(symbol->address, REDasm::trampoline(symimport->name));
    InstructionPtr instruction = m_document->instruction(symbol->address);

    if(!instruction)
        return;

    m_disassembler->pushReference(symimport->address, instruction->address);
}

SymbolPtr Analyzer::findTrampolines_x86(ListingDocument::iterator it)
{
    InstructionPtr instruction = m_disassembler->document()->instruction((*it)->address);

    if(!instruction->is(InstructionTypes::Jump) || !instruction->hasTargets())
        return NULL;

    return m_disassembler->document()->symbol(instruction->target());
}

SymbolPtr Analyzer::findTrampolines_arm(ListingDocument::iterator it)
{
    ListingDocument* doc = m_disassembler->document();
    InstructionPtr instruction1 = doc->instruction((*it)->address);
    it++;

    if(it == doc->end() || (*it)->type != ListingItem::InstructionItem)
        return NULL;

    const InstructionPtr& instruction2 = doc->instruction((*it)->address);

    if(!instruction1 || !instruction2 || instruction1->isInvalid() || instruction2->isInvalid())
        return NULL;

    if((instruction1->mnemonic != "ldr") && (instruction2->mnemonic != "ldr"))
        return NULL;

    if(!instruction1->operands[1].is(OperandTypes::Memory) && (instruction2->operands[0].reg.r != ARM_REG_PC))
        return NULL;

    u64 target = instruction1->operands[1].u_value, importaddress = 0;

    if(!m_disassembler->readAddress(target, sizeof(u32), &importaddress))
        return NULL;

    SymbolPtr symbol = doc->symbol(target), impsymbol = doc->symbol(importaddress);

    if(symbol && impsymbol)
        doc->lock(symbol->address, "imp." + impsymbol->name);

    return impsymbol;
}

} // namespace REDasm
