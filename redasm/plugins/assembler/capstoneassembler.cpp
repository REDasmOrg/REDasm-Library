#include "capstoneassembler.h"
#include <impl/plugins/assembler/capstoneassembler_impl.h>

namespace REDasm {

CapstoneAssembler::CapstoneAssembler(int arch, int mode): Assembler(new CapstoneAssemblerImpl(arch, mode)) { }

bool CapstoneAssembler::decodeInstruction(const BufferView &view, const InstructionPtr &instruction)
{
    PIMPL_P(CapstoneAssembler);

    address_t address = instruction->address;
    const u8* pdata = static_cast<const u8*>(view);
    size_t len = view.size();
    cs_insn* insn = cs_malloc(p->handle());

    if(!cs_disasm_iter(p->handle(), &pdata, &len, &address, insn))
        return false;

    instruction->mnemonic = insn->mnemonic;
    instruction->id = insn->id;
    instruction->size = insn->size;
    instruction->meta.userdata = insn;
    instruction->free = &CapstoneAssemblerImpl::free;
    return true;
}

size_t CapstoneAssembler::handle() const { PIMPL_P(const CapstoneAssembler); return p->m_handle; }

void CapstoneAssembler::onDecoded(const InstructionPtr &instruction)
{
    cs_insn* insn = reinterpret_cast<cs_insn*>(instruction->meta.userdata);

    if(!insn)
        return;

    PIMPL_P(CapstoneAssembler);

    if(cs_insn_group(p->handle(), insn, CS_GRP_JUMP))
        instruction->type |= InstructionType::Jump;
    else if(cs_insn_group(p->handle(), insn, CS_GRP_CALL))
        instruction->type |= InstructionType::Call;
    else if(cs_insn_group(p->handle(), insn, CS_GRP_RET))
        instruction->type |= InstructionType::Stop;
    else if(cs_insn_group(p->handle(), insn, CS_GRP_INT) || cs_insn_group(p->handle(), insn, CS_GRP_IRET))
        instruction->type |= InstructionType::Privileged;
}

} // namespace REDasm
