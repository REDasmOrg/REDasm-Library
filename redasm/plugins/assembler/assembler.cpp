#include "assembler.h"
#include "algorithm/controlflow.h"
#include "../loader.h"
#include <iomanip>
#include <sstream>

namespace REDasm {

AssemblerPlugin::AssemblerPlugin(): Plugin() { }
u32 AssemblerPlugin::flags() const { return AssemblerFlags::None; }
Emulator *AssemblerPlugin::createEmulator(DisassemblerAPI *disassembler) const { RE_UNUSED(disassembler); return nullptr; }
Printer *AssemblerPlugin::createPrinter(DisassemblerAPI *disassembler) const { return new Printer(disassembler); }
AssemblerAlgorithm *AssemblerPlugin::createAlgorithm(DisassemblerAPI *disassembler) { return new ControlFlowAlgorithm(disassembler, this); }
void AssemblerPlugin::onDecoded(const InstructionPtr &instruction) { RE_UNUSED(instruction); }

void AssemblerPlugin::setInstructionType(const InstructionPtr &instruction) const
{
    auto it = m_instructiontypes.find(instruction->id);

    if(it != m_instructiontypes.end())
        instruction->type |= it->second;
}

bool AssemblerPlugin::hasFlag(u32 flag) const { return this->flags() & flag; }
u64 AssemblerPlugin::addressWidth() const { return this->bits() / CHAR_BIT; }

bool AssemblerPlugin::decode(const BufferView& view, const InstructionPtr &instruction)
{
    bool decoded = this->decodeInstruction(view, instruction);

    if(!decoded || instruction->isInvalid())
        return false;

    this->setInstructionType(instruction);
    this->onDecoded(instruction);
    m_dispatcher(instruction->id, instruction);
    return true;
}

bool AssemblerPlugin::decodeInstruction(const BufferView &view, const InstructionPtr &instruction) { return false; }

}
