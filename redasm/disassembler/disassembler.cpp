#include "disassembler.h"
#include "../plugins/assembler/algorithm/algorithm.h"
#include <algorithm>
#include <memory>

#define DO_TICK_DISASSEMBLY()  m_cctimer.tick(std::bind(&Disassembler::disassembleStep, this, m_algorithm.get()))

namespace REDasm {

Disassembler::Disassembler(AssemblerPlugin *assembler, FormatPlugin *format): DisassemblerBase(format)
{
    if(!format->isBinary())
        assembler->setEndianness(format->endianness());

    m_assembler = std::unique_ptr<AssemblerPlugin>(assembler);
    m_algorithm = std::unique_ptr<AssemblerAlgorithm>(m_assembler->createAlgorithm(this));

    m_cctimer.stateChanged += [&](Timer*) { busyChanged(); };
}

Disassembler::~Disassembler() { }
ListingDocument *Disassembler::document() { return m_document; }

void Disassembler::disassembleStep(AssemblerAlgorithm* algorithm)
{
    if(!algorithm->hasNext())
    {
        m_cctimer.stop();
        algorithm->analyze();
        return;
    }

    algorithm->next();
}

void Disassembler::disassemble()
{
    if(!m_document->segmentsCount())
    {
        REDasm::log("ERROR: Segment list is empty");
        return;
    }

    SymbolTable* symboltable = m_document->symbols();

    // Preload format functions for analysis
    symboltable->iterate(SymbolTypes::FunctionMask, [=](SymbolPtr symbol) -> bool {
        m_algorithm->enqueue(symbol->address);
        return true;
    });

    SymbolPtr entrypoint = m_document->documentEntry();

    if(entrypoint)
        m_algorithm->enqueue(entrypoint->address); // Push entry point

    REDasm::log("Disassembling with " + std::to_string(m_cctimer.concurrency()) + " threads" +
                " (" + std::to_string(m_cctimer.interval()) + "ms interval)");

    DO_TICK_DISASSEMBLY();
}

Printer *Disassembler::createPrinter() { return m_assembler->createPrinter(this); }
AssemblerPlugin *Disassembler::assembler() { return m_assembler.get(); }

void Disassembler::disassemble(address_t address)
{
    m_algorithm->enqueue(address);

    if(m_cctimer.active())
        return;

    DO_TICK_DISASSEMBLY();
}

void Disassembler::pause() { m_cctimer.pause(); }
void Disassembler::resume() { m_cctimer.resume(); }
size_t Disassembler::state() const { return m_cctimer.state(); }
bool Disassembler::busy() const { return m_cctimer.active(); }

InstructionPtr Disassembler::disassembleInstruction(address_t address)
{
    InstructionPtr instruction = m_document->instruction(address);

    if(instruction)
        return instruction;

    instruction = std::make_shared<Instruction>();
    m_algorithm->disassembleInstruction(address, instruction);
    return instruction;
}

}
