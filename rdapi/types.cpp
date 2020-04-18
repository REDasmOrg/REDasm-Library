#include "types.h"
#include <rdcore/document/backend/segmentcontainer.h>
#include <algorithm>
#include <cstring>

size_t RDSegment_RawSize(const RDSegment* s) { return SegmentContainer::offsetSize(*s); }
size_t RDSegment_Size(const RDSegment* s) { return SegmentContainer::addressSize(*s); }

void RDInstruction_SetMnemonic(RDInstruction* instruction, const char* mnemonic)
{
    if(!mnemonic) return;
    std::copy_n(mnemonic, std::min<size_t>(std::strlen(mnemonic), sizeof(instruction->mnemonic)), instruction->mnemonic);
}

bool RDInstruction_PushOperand(RDInstruction* instruction, RDOperand* op)
{
    if(!instruction || (instruction->operandscount >= DEFAULT_CONTAINER_SIZE)) return false;

    op->pos = instruction->operandscount++;
    instruction->operands[op->pos] = *op;
    return true;
}
