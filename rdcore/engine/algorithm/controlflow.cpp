#include "controlflow.h"
#include "../../support/sugar.h"
#include "../../disassembler.h"
#include "../../context.h"

ControlFlowAlgorithm::ControlFlowAlgorithm(): Algorithm() { }

void ControlFlowAlgorithm::enqueueTargets(const RDInstruction* instruction)
{
    const address_t* targets = nullptr;
    size_t c = rd_disasm->getTargets(instruction->address, &targets);
    for(size_t i = 0; i < c; i++) this->enqueue(targets[i]);
}

void ControlFlowAlgorithm::addressTableState(const RDState* state)
{
    Algorithm::addressTableState(state);
    this->enqueueTargets(state->instruction);
}

void ControlFlowAlgorithm::onDecoded(const RDInstruction* instruction)
{
    Algorithm::onDecoded(instruction);
    this->enqueueTargets(instruction);
    if(instruction->type == InstructionType_Stop) return;

    switch(instruction->type)
    {
        case InstructionType_Jump:
            if(!(instruction->flags & InstructionFlags_Conditional)) return;
            break;

        default: break;
    }

    this->enqueue(Sugar::endAddress(instruction));
}
