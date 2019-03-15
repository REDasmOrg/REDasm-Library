#include "cil.h"

namespace REDasm {

u32 CILAssembler::bits() const { return 32; }
bool CILAssembler::decodeInstruction(const BufferView& view, const InstructionPtr &instruction) { return false; }

} // namespace REDasm
