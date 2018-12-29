#include "metaarm_isa.h"
#include "../../disassembler/disassemblerapi.h"

// https://static.docs.arm.com/ddi0406/c/DDI0406C_C_arm_architecture_reference_manual.pdf

namespace REDasm {

int MetaARMAssemblerISA::classify(address_t address, const BufferRef &buffer, DisassemblerAPI *disassembler, AssemblerPlugin *armassembler)
{
    BufferRef br = buffer;
    InstructionPtr instruction = std::make_shared<Instruction>();

    while(!br.eob())
    {
        REDasm::status("Classifing Instruction Set @ " + REDasm::hex(address));

        if(!armassembler->decode(br, instruction))
            return MetaARMAssemblerISA::Thumb;

        if(instruction->is(InstructionTypes::Stop) || (instruction->is(InstructionTypes::Jump) && !instruction->is(InstructionTypes::Conditional)))
            break;

        if(instruction->is(InstructionTypes::Branch) && !MetaARMAssemblerISA::validateBranch(instruction, disassembler))
            return MetaARMAssemblerISA::Thumb;

        address += instruction->size;
        br.advance(instruction->size);
        instruction->reset();
    }

    return MetaARMAssemblerISA::ARM;
}

bool MetaARMAssemblerISA::validateBranch(const InstructionPtr &instruction, DisassemblerAPI* disassembler)
{
    ListingDocument* document = disassembler->document();

    for(address_t target : instruction->targets)
    {
        if(!document->segment(target))
            return false;
    }

    return true;
}

} // namespace REDasm
