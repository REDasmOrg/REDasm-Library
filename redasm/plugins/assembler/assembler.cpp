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
    p->classify(instruction);
    this->onDecoded(instruction);
    p->m_dispatcher(instruction->id, instruction);
    return true;
}

bool Assembler::decodeInstruction(const BufferView &view, Instruction* instruction) { return false; }
const Symbol* Assembler::findTrampoline(size_t index) const { return nullptr; }
Algorithm *Assembler::doCreateAlgorithm() const { return new ControlFlowAlgorithm(); }
Printer *Assembler::doCreatePrinter() const { return new Printer(); }
void Assembler::classifyInstruction(instruction_id_t id, InstructionType type, InstructionFlags flags) { PIMPL_P(Assembler); p->m_classifiedinstruction[id] = { type, flags }; }
void Assembler::registerInstruction(instruction_id_t id, const InstructionCallback& cb) { PIMPL_P(Assembler); p->m_dispatcher[id] = cb; }
void Assembler::onDecoded(Instruction *instruction) { }
size_t Assembler::addressWidth() const { return this->bits() / CHAR_BIT; }
void Assembler::init(const AssemblerRequest& request) { PIMPL_P(Assembler); p->init(request); }

} // namespace REDasm
