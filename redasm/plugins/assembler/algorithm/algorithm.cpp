#include "algorithm.h"
#include <impl/plugins/assembler/algorithm/algorithm_impl.h>
#include "../../../plugins/assembler/assembler.h"
#include "../../../disassembler/disassembler.h"
#include "../../../support/utils.h"
#include "../../../context.h"

namespace REDasm {

Algorithm::Algorithm(AlgorithmImpl *p): m_pimpl_p(p) { }
Algorithm::Algorithm(): m_pimpl_p(new AlgorithmImpl(this)) { }
void Algorithm::enqueue(address_t address) { PIMPL_P(Algorithm); p->enqueue(address); }
bool Algorithm::hasNext() const { PIMPL_P(const Algorithm); return p->hasNext(); }
void Algorithm::next() { PIMPL_P(Algorithm); p->next(); }
void Algorithm::registerState(state_t id, const StateCallback &cb) { PIMPL_P(Algorithm); p->registerState(id, cb); }
void Algorithm::enqueueState(const State &state) { PIMPL_P(Algorithm); p->enqueueState(state); }
void Algorithm::executeState(const State &state) { PIMPL_P(Algorithm); p->executeState(state); }
void Algorithm::validateTarget(const CachedInstruction& instruction) const { PIMPL_P(const Algorithm); p->validateTarget(instruction);  }
bool Algorithm::validateState(const State &state) const { PIMPL_P(const Algorithm); return p->validateState(state); }
void Algorithm::onNewState(const State* state) const { PIMPL_P(const Algorithm); return p->onNewState(state); }
CachedInstruction Algorithm::decodeInstruction(address_t address) { PIMPL_P(Algorithm); return p->decodeInstruction(address); }

void Algorithm::onDecoded(const CachedInstruction& instruction)
{
    PIMPL_P(Algorithm);

    if(instruction->typeIs(InstructionType::Branch))
    {
        p->loadTargets(instruction);
        this->validateTarget(instruction);
    }

    for(size_t i = 0; i < instruction->operandscount; i++)
    {
        const auto* op = &instruction->operandsstruct[i];

        if(!op->isNumeric() || op->displacementIsDynamic())
        {
            if(!REDasm::typeIs(op, OperandType::Displacement)) // Try static displacement analysis
                continue;
        }

        if(REDasm::typeIs(op, OperandType::Displacement))
        {
            if(op->displacementIsDynamic())
                EXECUTE_STATE(Algorithm::AddressTableState, op->disp.displacement, op->index, instruction);
            else if(op->displacementCanBeAddress())
                EXECUTE_STATE(Algorithm::MemoryState, op->disp.displacement, op->index, instruction);
        }
        else if(REDasm::typeIs(op, OperandType::Memory))
            EXECUTE_STATE(Algorithm::MemoryState, op->u_value, op->index, instruction);
        else if(REDasm::typeIs(op, OperandType::Immediate))
            EXECUTE_STATE(Algorithm::ImmediateState, op->u_value, op->index, instruction);

        this->onDecodedOperand(op, instruction);
    }
}

void Algorithm::onDecodeFailed(const CachedInstruction& instruction)
{
    r_ctx->problem("Invalid instruction @ " + String::hex(instruction->address));

    if(!instruction->size) return;
    this->enqueue(instruction->endAddress());
}

void Algorithm::onDecodedOperand(const Operand *op, const CachedInstruction &instruction)
{
    if(!op->isCharacter()) return;
    String charinfo = String::hex(op->u_value, 8, true) + "=" + String(static_cast<char>(op->u_value)).quotedSingle();
    r_doc->autoComment(instruction->address, charinfo);
}

void Algorithm::decodeState(const State *state) { PIMPL_P(Algorithm); p->decode(state->address); }

void Algorithm::jumpState(const State *state)
{
    int dir = BRANCH_DIRECTION(state->instruction, state->address);
    if(!dir) r_doc->autoComment(state->instruction->address, "Infinite loop");

    r_doc->branch(state->address, dir);
    DECODE_STATE(state->address);
}

void Algorithm::callState(const State *state) { r_doc->function(state->address); }

void Algorithm::branchState(const State *state)
{
    CachedInstruction instruction = state->instruction;

    if(instruction->typeIs(InstructionType::Call))
        FORWARD_STATE(Algorithm::CallState, state);
    else if(instruction->typeIs(InstructionType::Jump))
        FORWARD_STATE(Algorithm::JumpState, state);
    else
    {
        r_ctx->problem("Invalid branch state for instruction " + instruction->mnemonic().quoted() +
                       " @ " + String::hex(instruction->address, r_asm->bits()));
        return;
    }

    r_disasm->pushReference(state->address, instruction->address);
    r_disasm->pushTarget(state->address, instruction->address);
}

void Algorithm::branchMemoryState(const State *state)
{
    CachedInstruction instruction = state->instruction;
    r_disasm->pushTarget(state->address, instruction->address);

    const Symbol* symbol = r_doc->symbol(state->address);

    if(symbol && symbol->isImport()) // Don't dereference imports
        return;

    u64 value = 0;
    r_disasm->dereference(state->address, &value);
    r_doc->pointer(state->address);

    if(instruction->typeIs(InstructionType::Call)) r_doc->function(value);
    else r_doc->label(value);

    r_disasm->pushReference(value, state->address);
}

void Algorithm::addressTableState(const State *state)
{
    CachedInstruction instruction = state->instruction;
    size_t c = r_disasm->checkAddressTable(instruction, state->address);
    if(c == REDasm::npos) return;

    if(c > 1)
    {
        r_disasm->pushReference(state->address, instruction->address);
        state_t fwdstate = Algorithm::BranchState;

        if(instruction->typeIs(InstructionType::Call))
            r_doc->autoComment(instruction->address, "Call Table with " + String::number(c) + " cases(s)");
        else if(instruction->typeIs(InstructionType::Jump))
            r_doc->autoComment(instruction->address, "Jump Table with " + String::number(c) + " cases(s)");
        else
        {
            r_doc->autoComment(instruction->address, "Address Table with " + String::number(c) + " cases(s)");
            fwdstate = Algorithm::MemoryState;
        }

        SortedSet targets = r_disasm->getTargets(instruction->address);
        targets.each([&](const Variant& v) { FORWARD_STATE_VALUE(fwdstate, v.toU64(), state); });
        return;
    }

    const Operand* op = state->operand();

    if(REDasm::typeIs(op, OperandType::Displacement)) FORWARD_STATE(Algorithm::PointerState, state);
    else if(REDasm::typeIs(op, OperandType::Memory)) FORWARD_STATE(Algorithm::MemoryState, state);
    else FORWARD_STATE(Algorithm::ImmediateState, state);
}

void Algorithm::memoryState(const State *state)
{
    u64 value = 0;

    if(!r_disasm->dereference(state->address, &value))
    {
        FORWARD_STATE(Algorithm::ImmediateState, state);
        return;
    }

    CachedInstruction instruction = state->instruction;
    r_disasm->pushReference(state->address, instruction->address);

    if(instruction->typeIs(InstructionType::Branch) && state->operand()->isTarget())
        FORWARD_STATE(Algorithm::BranchMemoryState, state);
    else
        FORWARD_STATE(Algorithm::PointerState, state);
}

void Algorithm::pointerState(const State *state)
{
    u64 value = 0;

    if(!r_disasm->dereference(state->address, &value))
    {
        FORWARD_STATE(Algorithm::ImmediateState, state);
        return;
    }

    r_doc->pointer(state->address);
    r_disasm->checkLocation(state->address, value); // Create Symbol + XRefs

    const Symbol* symbol = r_doc->symbol(value);
    if(!symbol) return;

    if(symbol->isString())
    {
        if(symbol->isWideString()) r_doc->autoComment(state->instruction->address, "=> WIDE STRING: " + r_disasm->readWString(value).quoted());
        else r_doc->autoComment(state->instruction->address, "=> STRING: " + r_disasm->readString(value).quoted());
    }
    else if(symbol->isImport()) r_doc->autoComment(state->instruction->address, "=> IMPORT: " + symbol->name);
    else if(symbol->isExport()) r_doc->autoComment(state->instruction->address, "=> EXPORT: " + symbol->name);
    else return;

    r_disasm->pushReference(value, state->instruction->address);
}

void Algorithm::immediateState(const State *state)
{
    CachedInstruction instruction = state->instruction;

    if(instruction->typeIs(InstructionType::Branch) && state->operand()->isTarget())
        FORWARD_STATE(Algorithm::BranchState, state);
    else
        r_disasm->checkLocation(instruction->address, state->address); // Create Symbol + XRefs
}

} // namespace REDasm
