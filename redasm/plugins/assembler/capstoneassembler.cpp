#include "capstoneassembler.h"
#include <impl/plugins/assembler/capstoneassembler_impl.h>

namespace REDasm {

CapstoneAssembler::CapstoneAssembler(): Assembler(new CapstoneAssemblerImpl()) { }

bool CapstoneAssembler::decodeInstruction(const BufferView &view, Instruction *instruction)
{
    PIMPL_P(CapstoneAssembler);

    address_t address = instruction->address;
    const u8* pdata = view.data();
    size_t len = view.size();
    cs_insn* insn = cs_malloc(p->handle());

    if(!cs_disasm_iter(p->handle(), &pdata, &len, &address, insn))
        return false;

    instruction->mnemonic(insn->mnemonic);
    instruction->id = insn->id;
    instruction->size = insn->size;
    instruction->puserdata = insn;
    instruction->free = &CapstoneAssemblerImpl::free;
    return true;
}

size_t CapstoneAssembler::handle() const { PIMPL_P(const CapstoneAssembler); return p->m_handle; }
int CapstoneAssembler::arch() const { PIMPL_P(const CapstoneAssembler); return p->arch(); }
int CapstoneAssembler::mode() const { PIMPL_P(const CapstoneAssembler); return p->mode(); }
void CapstoneAssembler::open(int arch, int mode) { PIMPL_P(CapstoneAssembler); p->open(arch, mode); }

void CapstoneAssembler::onDecoded(Instruction *instruction)
{
    cs_insn* insn = reinterpret_cast<cs_insn*>(instruction->userdata);
    if(!insn) return;

    PIMPL_P(CapstoneAssembler);

    if(cs_insn_group(p->handle(), insn, CS_GRP_JUMP)) instruction->type |= InstructionType::Jump;
    else if(cs_insn_group(p->handle(), insn, CS_GRP_CALL)) instruction->type |= InstructionType::Call;
    else if(cs_insn_group(p->handle(), insn, CS_GRP_RET)) instruction->type |= InstructionType::Stop;
    else if(cs_insn_group(p->handle(), insn, CS_GRP_INT) || cs_insn_group(p->handle(), insn, CS_GRP_IRET))
        instruction->type |= InstructionType::Privileged;
}

} // namespace REDasm
