#include "assembler.h"
#include <impl/plugins/assembler/assembler_impl.h>
#include "algorithm/controlflow.h"
#include <climits>

namespace REDasm {

Assembler::Assembler(AssemblerImpl *p): Plugin(p) { }
Assembler::Assembler(): Plugin(new AssemblerImpl()) { }
const AssemblerRequest& Assembler::request() const { PIMPL_P(const Assembler); return p->request(); }

bool Assembler::decode(const BufferView &view, Instruction* instruction)
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

bool Assembler::decodeInstruction(const BufferView &view, Instruction* instruction) { return false; }
Symbol *Assembler::findTrampoline(ListingDocumentIterator *it) const { return nullptr; }
Algorithm *Assembler::doCreateAlgorithm(Disassembler *disassembler) const { return new ControlFlowAlgorithm(disassembler); }
Printer *Assembler::doCreatePrinter(Disassembler *disassembler) const { return new Printer(disassembler); }
void Assembler::setInstructionType(instruction_id_t id, InstructionType type) { PIMPL_P(Assembler); p->m_instructiontypes[id] = type; }
void Assembler::registerInstruction(instruction_id_t id, const InstructionCallback& cb) { PIMPL_P(Assembler); p->m_dispatcher[id] = cb; }
void Assembler::onDecoded(Instruction *instruction) { }
size_t Assembler::addressWidth() const { return this->bits() / CHAR_BIT; }
void Assembler::init(const AssemblerRequest& request) { PIMPL_P(Assembler); p->init(request); }

} // namespace REDasm
