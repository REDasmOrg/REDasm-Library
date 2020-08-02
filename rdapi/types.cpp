#include "types.h"
#include <rdcore/document/backend/segmentcontainer.h>
#include <rdcore/support/sugar.h>
#include <algorithm>
#include <cstring>

size_t RDSegment_RawSize(const RDSegment* s) { return SegmentContainer::offsetSize(*s); }
size_t RDSegment_Size(const RDSegment* s) { return SegmentContainer::addressSize(*s); }
rd_address RDInstruction_NextAddress(const RDInstruction* instruction) { return instruction->address + instruction->size; }
void RDInstruction_SetMnemonic(RDInstruction* instruction, const char* mnemonic) { Sugar::setMnemonic(instruction, mnemonic); }

RDOperand* RDInstruction_PushOperand(RDInstruction* instruction, rd_type type)
{
    if(!instruction || (instruction->operandscount >= DEFAULT_CONTAINER_SIZE)) return nullptr;

    RDOperand op{ };
    op.type = type;

    // Invalidate registers
    op.base = RD_NPOS;
    op.index = RD_NPOS;

    size_t pos = instruction->operandscount++;
    instruction->operands[pos] = op;
    return &instruction->operands[pos];
}

bool RDInstruction_MnemonicIs(const RDInstruction* instruction, const char* mnemonic)
{
    if(!mnemonic || (std::strlen(mnemonic) > DEFAULT_NAME_SIZE)) return false;
    return !std::strcmp(instruction->mnemonic, mnemonic);
}

void RDInstruction_PopOperand(RDInstruction* instruction, size_t idx)
{
    if(!instruction || (idx >= instruction->operandscount)) return;

    for(size_t i = idx; i < instruction->operandscount; i++)
        instruction->operands[i] = instruction->operands[i + 1];

    instruction->operandscount--;
    instruction->operands[instruction->operandscount - 1] = { };
}

void RDInstruction_ClearOperands(RDInstruction* instruction)
{
    std::fill_n(std::begin(instruction->operands), instruction->operandscount, RDOperand{ });
    instruction->operandscount = 0;
}

RDOperand* RDInstruction_FirstOperand(RDInstruction* instruction)
{
    if(!instruction->operandscount) return nullptr;
    return &instruction->operands[0];
}

RDOperand* RDInstruction_LastOperand(RDInstruction* instruction)
{
    if(!instruction->operandscount) return nullptr;
    return &instruction->operands[instruction->operandscount - 1];
}

bool RDSegment_ContainsAddress(const RDSegment* s, rd_address address) { return SegmentContainer::containsAddress(s, address); }
bool RDSegment_ContainsOffset(const RDSegment* s, rd_offset offset) { return SegmentContainer::containsOffset(s, offset); }
