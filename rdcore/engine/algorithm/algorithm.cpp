#include "algorithm.h"
#include "../../document/document.h"
#include "../../support/utils.h"
#include "../../support/sugar.h"
#include "../../disassembler.h"
#include "../../context.h"
#include <cassert>

#define REGISTER_STATE(id, cb)                       m_states[id] = std::bind(cb, this, std::placeholders::_1)
#define EXECUTE_STATE(id, value, index, instruction) this->executeState({ #id, id, {u64(value)}, index, instruction })
#define FORWARD_STATE_VALUE(newid, value, state)     EXECUTE_STATE(newid, value, state->opindex, state->instruction)
#define FORWARD_STATE(newid, state)                  FORWARD_STATE_VALUE(newid, state->u_value, state)

Algorithm::Algorithm()
{
    REGISTER_STATE(Algorithm::State_Decode,       &Algorithm::decodeState);
    REGISTER_STATE(Algorithm::State_Jump,         &Algorithm::jumpState);
    REGISTER_STATE(Algorithm::State_Call,         &Algorithm::callState);
    REGISTER_STATE(Algorithm::State_Branch,       &Algorithm::branchState);
    REGISTER_STATE(Algorithm::State_BranchMemory, &Algorithm::branchMemoryState);
    REGISTER_STATE(Algorithm::State_AddressTable, &Algorithm::addressTableState);
    REGISTER_STATE(Algorithm::State_Memory,       &Algorithm::memoryState);
    REGISTER_STATE(Algorithm::State_Pointer,      &Algorithm::pointerState);
    REGISTER_STATE(Algorithm::State_Immediate,    &Algorithm::immediateState);
}

const RDInstruction* Algorithm::decodeInstruction(address_t address)
{
    //if(rd_doc->isInstructionCached(address)) return rd_doc->instruction(address, nullptr);
    //return this->decode(address);
}

void Algorithm::enqueue(address_t address) { DECODE_STATE(address); }

size_t Algorithm::decode(address_t address, RDInstruction* instruction)
{
    if(!this->canBeDisassembled(address)) return Algorithm::SKIP;
    instruction->address = address;

    std::unique_ptr<BufferView> view(rd_ldr->view(address));
    if(!view || view->empty()) return Algorithm::SKIP;
    return rd_disasm->decode(view.get(), instruction) ? Algorithm::OK : Algorithm::FAIL;
}

bool Algorithm::canBeDisassembled(address_t address)
{
    if(rd_doc->isInstructionCached(address)) return false;

    if(!rd_doc->segment(address, &m_currentsegment)|| !(m_currentsegment.type & SegmentType_Code))
        return false;

    RDBlock b;
    assert(rd_doc->block(address, &b));

    if(b.type == BlockType_Code) return false;

    if(b.type == BlockType_Data)
    {
        RDSymbol symbol;
        assert(rd_doc->symbol(b.start, &symbol));

        switch(symbol.type)
        {
            case SymbolType_Label:
            case SymbolType_Function: return true;
            default: break;
        }

        return symbol.flags & SymbolFlags_Weak;
    }

    return true;
}

void Algorithm::onDecodedOperand(const RDOperand* op, const RDInstruction* instruction)
{
    if(!Sugar::isCharacter(op)) return;
    std::string charinfo = Utils::hex(op->u_value, 8, true) + "=" + Utils::quotedSingle(std::string(1, static_cast<char>(op->u_value)));
    rd_doc->autoComment(instruction->address, charinfo);
}

void Algorithm::onDecodeFailed(const RDInstruction* instruction)
{
    rd_ctx->problem("Invalid instruction @ " + Utils::hex(instruction->address));

    if(!instruction->size) return;
    this->enqueue(Sugar::endAddress(instruction));
}

void Algorithm::onDecoded(const RDInstruction* instruction)
{
    if(Sugar::isBranch(instruction))
    {
        for(size_t i = 0; i < instruction->operandscount; i++)
        {
            const RDOperand& op = instruction->operands[i];
            if(!Sugar::isTarget(&op) || !Sugar::isNumeric(&op)) continue;
            rd_disasm->pushTarget(op.u_value, instruction->address);
        }

        this->validateTarget(instruction);
    }

    for(size_t i = 0; i < instruction->operandscount; i++)
    {
        const RDOperand* op = &instruction->operands[i];

        if(!Sugar::isNumeric(op) || Sugar::displacementIsDynamic(op))
        {
            if(op->type != OperandType_Displacement) // Try static displacement analysis
                continue;
        }

        if(op->type == OperandType_Displacement)
        {
            if(Sugar::displacementIsDynamic(op))
                EXECUTE_STATE(Algorithm::State_AddressTable, op->displacement, op->pos, instruction);
            else if(Sugar::displacementCanBeAddress(op))
                EXECUTE_STATE(Algorithm::State_Memory, op->displacement, op->pos, instruction);
        }
        else if(op->type == OperandType_Memory)
            EXECUTE_STATE(Algorithm::State_Memory, op->u_value, op->pos, instruction);
        else if(op->type == OperandType_Immediate)
            EXECUTE_STATE(Algorithm::State_Immediate, op->u_value, op->pos, instruction);

        this->onDecodedOperand(op, instruction);
    }
}

void Algorithm::decode(address_t address)
{
    RDInstruction instruction{ };
    size_t result = this->decode(address, &instruction);

    switch(result)
    {
        case Algorithm::OK:
            this->onDecoded(&instruction);
            break;

        case Algorithm::FAIL:
            this->invalidInstruction(&instruction);
            this->onDecodeFailed(&instruction);
            break;

        default: return;
    }

    rd_doc->instruction(&instruction);
}

void Algorithm::validateTarget(const RDInstruction* instruction) const
{
    if(rd_disasm->getTargetsCount(instruction->address)) return;

    const RDOperand* op = Sugar::target(instruction);
    if(op && !Sugar::isNumeric(op)) return;

    rd_ctx->problem("No targets found for " + Utils::quoted(instruction->mnemonic) + " @ " + Utils::hex(instruction->address));
}

void Algorithm::invalidInstruction(RDInstruction* instruction) const
{
    if(!instruction->size)
        instruction->size =1; // Invalid instruction uses at least 1 byte

    instruction->type = InstructionType_Invalid;
    instruction->mnemonic[0] = 'd';
    instruction->mnemonic[1] = 'b';
    instruction->mnemonic[2] = '\0';
}

void Algorithm::decodeState(const RDState* state) { this->decode(state->address); }

void Algorithm::jumpState(const RDState* state)
{
    int dir = Sugar::branchDirection(state->instruction, state->address);
    if(!dir) rd_doc->autoComment(state->instruction->address, "Infinite loop");

    rd_doc->branch(state->address, dir);
    DECODE_STATE(state->address);
}

void Algorithm::callState(const RDState* state)
{
    rd_doc->function(state->address, std::string());
    DECODE_STATE(state->address);
}

void Algorithm::branchState(const RDState* state)
{
    const RDInstruction* instruction = state->instruction;

    switch(instruction->type)
    {
        case InstructionType_Call: FORWARD_STATE(Algorithm::State_Call, state); break;
        case InstructionType_Jump: FORWARD_STATE(Algorithm::State_Jump, state); break;

        default:
            rd_ctx->problem("Invalid branch state for instruction " + Utils::quoted(instruction->mnemonic) +
                            " @ " + Utils::hex(instruction->address, rd_disasm->bits()));
            return;
    }

    rd_disasm->pushReference(state->address, instruction->address);
    rd_disasm->pushTarget(state->address, instruction->address);
}

void Algorithm::branchMemoryState(const RDState* state)
{
    const RDInstruction* instruction = state->instruction;
    rd_disasm->pushTarget(state->address, instruction->address);

    RDSymbol symbol;
    if(rd_doc->symbol(state->address, &symbol) && (symbol.type == SymbolType_Import)) return; // Don't dereference imports

    RDLocation loc = rd_disasm->dereference(state->address);
    if(!loc.valid) return;

    rd_doc->pointer(state->address, SymbolType_Data, std::string());

    if(instruction->type == InstructionType_Call) rd_doc->function(loc.value, std::string());
    else rd_doc->label(loc.value);

    rd_disasm->pushReference(loc.value, state->address);
    DECODE_STATE(loc.value);
}

void Algorithm::addressTableState(const RDState* state)
{
    const RDInstruction* instruction = state->instruction;
    size_t c = rd_disasm->markTable(instruction, state->address);
    if(c == RD_NPOS) return;

    if(c > 1)
    {
        rd_disasm->pushReference(state->address, instruction->address);
        state_t fwdstate = Algorithm::State_Branch;

        switch(instruction->type)
        {
            case InstructionType_Call:
                rd_doc->autoComment(instruction->address, "Call Table with " + Utils::number(c) + " cases(s)");
                break;

            case InstructionType_Jump:
                rd_doc->autoComment(instruction->address, "Jump Table with " + Utils::number(c) + " cases(s)");
                break;

            default:
                rd_doc->autoComment(instruction->address, "Address Table with " + Utils::number(c) + " cases(s)");
                fwdstate = Algorithm::State_Memory;
                break;
        }

        const address_t* targets = nullptr;
        size_t c = rd_disasm->getTargets(instruction->address, &targets);
        for(size_t i = 0; i < c; i++) FORWARD_STATE_VALUE(fwdstate, targets[i], state);
        return;
    }

    const RDOperand* op = &instruction->operands[state->opindex];

    switch(op->type)
    {
        case OperandType_Displacement: FORWARD_STATE(Algorithm::State_Pointer, state); break;
        case OperandType_Memory: FORWARD_STATE(Algorithm::State_Memory, state); break;
        default: FORWARD_STATE(Algorithm::State_Immediate, state); break;
    }
}

void Algorithm::memoryState(const RDState* state)
{
    RDLocation loc = rd_disasm->dereference(state->address);

    if(!loc.valid)
    {
        FORWARD_STATE(Algorithm::State_Immediate, state);
        return;
    }

    const RDInstruction* instruction = state->instruction;
    const RDOperand* op = &instruction->operands[state->opindex];
    rd_disasm->pushReference(state->address, instruction->address);

    if(Sugar::isBranch(instruction) && (op->flags & OperandFlags_Target))
        FORWARD_STATE(Algorithm::State_BranchMemory, state);
    else
        FORWARD_STATE(Algorithm::State_Pointer, state);
}

void Algorithm::pointerState(const RDState* state)
{
    RDLocation loc = rd_disasm->dereference(state->address);

    if(!loc.valid)
    {
        FORWARD_STATE(Algorithm::State_Immediate, state);
        return;
    }

    rd_doc->pointer(state->address, SymbolType_Data, std::string());
    rd_disasm->markLocation(state->address, loc.value); // Create Symbol + XRefs

    RDSymbol symbol;
    if(!rd_doc->symbol(loc.value, &symbol)) return;

    if(symbol.type == SymbolType_String)
    {
        if(symbol.flags == SymbolFlags_WideString) rd_doc->autoComment(state->instruction->address, "=> WIDE STRING: " + Utils::quoted(rd_disasm->readWString(loc.value)));
        else rd_doc->autoComment(state->instruction->address, "=> STRING: " + Utils::quoted(rd_disasm->readWString(loc.value)));
    }
    else if(symbol.type & SymbolType_Import)
        rd_doc->autoComment(state->instruction->address, std::string("=> IMPORT: ") + rd_doc->name(symbol.address));
    else if(symbol.flags & SymbolFlags_Export)
        rd_doc->autoComment(state->instruction->address, std::string("=> EXPORT: ") + rd_doc->name(symbol.address));
    else
        return;

    rd_disasm->pushReference(loc.value, state->instruction->address);
}

void Algorithm::immediateState(const RDState* state)
{
    const RDInstruction* instruction = state->instruction;
    const RDOperand* op = &instruction->operands[state->opindex];

    if(Sugar::isBranch(instruction) && (op->flags & OperandFlags_Target))
        FORWARD_STATE(Algorithm::State_Branch, state);
    else
        rd_disasm->markLocation(instruction->address, state->address); // Create Symbol + XRefs
}
