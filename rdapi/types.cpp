#include "types.h"
#include <rdcore/document/backend/segmentcontainer.h>
#include <rdcore/support/sugar.h>
#include <algorithm>
#include <cstring>

size_t RDSegment_RawSize(const RDSegment* s) { return SegmentContainer::offsetSize(*s); }
size_t RDSegment_Size(const RDSegment* s) { return SegmentContainer::addressSize(*s); }

void RDInstruction_SetMnemonic(RDInstruction* instruction, const char* mnemonic)
{
    if(!mnemonic) return;
    std::copy_n(mnemonic, std::min<size_t>(std::strlen(mnemonic), sizeof(instruction->mnemonic)), instruction->mnemonic);
}

address_t RDInstruction_NextAddress(const RDInstruction* instruction) { return instruction->address + instruction->size; }

RDOperand* RDInstruction_PushOperand(RDInstruction* instruction, type_t type)
{
    if(!instruction || (instruction->operandscount >= DEFAULT_CONTAINER_SIZE)) return nullptr;

    RDOperand op{ };
    op.type = type;
    op.pos = instruction->operandscount++;
    instruction->operands[op.pos] = op;
    return &instruction->operands[op.pos];
}

bool RDInstruction_MnemonicIs(const RDInstruction* instruction, const char* mnemonic)
{
    if(!mnemonic || (std::strlen(mnemonic) > DEFAULT_NAME_SIZE)) return false;
    return !std::strcmp(instruction->mnemonic, mnemonic);
}

void RDInstruction_PopOperand(RDInstruction* instruction, size_t idx)
{
    if(!instruction || (idx >= instruction->operandscount)) return;

    auto endit = std::remove_if(std::begin(instruction->operands),
                                std::end(instruction->operands),
                                [idx](const RDOperand& op) { return op.pos == idx; });

    std::for_each(endit, std::end(instruction->operands), [](RDOperand& op) { op = { }; });
    instruction->operandscount--;

    // Recalculate positions
    for(size_t i = 0; i < instruction->operandscount; i++)
        instruction->operands[i].pos = i;
}

void RDInstruction_ClearOperands(RDInstruction* instruction)
{
    std::fill_n(std::begin(instruction->operands), instruction->operandscount, RDOperand{ });
    instruction->operandscount = 0;
}
