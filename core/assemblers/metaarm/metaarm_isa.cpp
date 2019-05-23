#include "metaarm_isa.h"
#include "../../disassembler/disassemblerapi.h"

// https://static.docs.arm.com/ddi0406/c/DDI0406C_C_arm_architecture_reference_manual.pdf

namespace REDasm {

int MetaARMAssemblerISA::classify(address_t address, const BufferView &view, DisassemblerAPI *disassembler, AssemblerPlugin *armassembler)
{
    BufferView cview = view;
    InstructionPtr instruction = std::make_shared<Instruction>();

    while(!cview.eob())
    {
        REDasm::statusAddress("Classifing Instruction Set", address);

        if(!armassembler->decode(cview, instruction))
            return MetaARMAssemblerISA::Thumb;

        if(instruction->is(InstructionType::Stop) || (instruction->is(InstructionType::Jump) && !instruction->is(InstructionType::Conditional)))
            break;

        if(instruction->is(InstructionType::Branch) && !MetaARMAssemblerISA::validateBranch(instruction, disassembler))
            return MetaARMAssemblerISA::Thumb;

        address += instruction->size;
        cview += instruction->size;
        instruction->reset();
    }

    return MetaARMAssemblerISA::ARM;
}

bool MetaARMAssemblerISA::validateBranch(const InstructionPtr &instruction, DisassemblerAPI* disassembler)
{
    ReferenceSet targets = disassembler->getTargets(instruction->address);
    auto& document = disassembler->document();

    for(address_t target : targets)
    {
        if(!document->segment(target))
            return false;
    }

    return true;
}

} // namespace REDasm
