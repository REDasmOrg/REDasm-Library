#include "assembler.h"
#include <impl/plugins/assembler/assembler_impl.h>
#include "algorithm/controlflow.h"
#include <climits>

namespace REDasm {

Assembler::Assembler(AssemblerImpl *p): Plugin(p) { }
Assembler::Assembler(): Plugin(new AssemblerImpl()) { }

bool Assembler::decode(const BufferView &view, const InstructionPtr &instruction)
{
    bool decoded = this->decodeInstruction(view, instruction);

    if(!decoded || instruction->isInvalid())
        return false;

    PIMPL_P(Assembler);
    p->setInstructionType(instruction);
    this->onDecoded(instruction);
    p->m_dispatcher(instruction->id, instruction);
    return true;
}

bool Assembler::decodeInstruction(const BufferView &view, const InstructionPtr &instruction) { return false; }
Symbol *Assembler::findTrampoline(ListingDocumentIterator *it) const { return nullptr; }
Algorithm *Assembler::doCreateAlgorithm(Disassembler *disassembler) const { return new ControlFlowAlgorithm(disassembler); }
Printer *Assembler::doCreatePrinter(Disassembler *disassembler) const { return new Printer(disassembler); }
void Assembler::setInstructionType(instruction_id_t id, InstructionType type) { PIMPL_P(Assembler); p->m_instructiontypes[id] = type; }
void Assembler::onDecoded(const InstructionPtr &instruction) { }
size_t Assembler::addressWidth() const { return this->bits() * CHAR_BIT; }

} // namespace REDasm
