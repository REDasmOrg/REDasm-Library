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
void Algorithm::analyze() { PIMPL_P(Algorithm); p->analyze(); }
bool Algorithm::hasNext() const { PIMPL_P(const Algorithm); return p->hasNext(); }
void Algorithm::next() { PIMPL_P(Algorithm); p->next(); }
void Algorithm::registerState(state_t id, const StateCallback &cb) { PIMPL_P(Algorithm); p->registerState(id, cb); }
void Algorithm::enqueueState(const State &state) { PIMPL_P(Algorithm); p->enqueueState(state); }
void Algorithm::executeState(const State &state) { PIMPL_P(Algorithm); p->executeState(state); }
void Algorithm::validateTarget(const CachedInstruction& instruction) const { PIMPL_P(const Algorithm); p->validateTarget(instruction);  }
bool Algorithm::validateState(const State &state) const { PIMPL_P(const Algorithm); return p->validateState(state); }
void Algorithm::onNewState(const State* state) const { PIMPL_P(const Algorithm); return p->onNewState(state); }
size_t Algorithm::disassembleInstruction(address_t address, const CachedInstruction& instruction) { PIMPL_P(Algorithm); return p->disassembleInstruction(address, instruction); }
void Algorithm::done(address_t address) { PIMPL_P(Algorithm); p->done(address); }

void Algorithm::onDecoded(const CachedInstruction& instruction)
{
    PIMPL_P(Algorithm);

    if(instruction->is(InstructionType::Branch))
    {
        p->loadTargets(instruction);
        this->validateTarget(instruction);
    }

    for(size_t i = 0; i < instruction->operandsCount(); i++)
    {
        const Operand* op = instruction->op(i);

        if(!op->isNumeric() || op->displacementIsDynamic())
        {
            //if(m_emulator && !m_emulator->hasError())
            //    this->emulateOperand(&op, instruction);

            if(!op->typeIs(OperandType::Displacement)) // Try static displacement analysis
                continue;
        }

        if(op->typeIs(OperandType::Displacement))
        {
            if(op->displacementIsDynamic())
                EXECUTE_STATE(Algorithm::AddressTableState, op->disp.displacement, op->index, instruction);
            else if(op->displacementCanBeAddress())
                EXECUTE_STATE(Algorithm::MemoryState, op->disp.displacement, op->index, instruction);
        }
        else if(op->typeIs(OperandType::Memory))
            EXECUTE_STATE(Algorithm::MemoryState, op->u_value, op->index, instruction);
        else if(op->typeIs(OperandType::Immediate))
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
    r_docnew->autoComment(instruction->address, charinfo);
}

void Algorithm::onEmulatedOperand(const Operand *op, const CachedInstruction &instruction, u64 value)
{
    const Segment* segment = r_docnew->segment(value);

    if(!segment || segment->isPureCode()) // Don't flood "Pure-Code" segments with symbols
        return;

    EXECUTE_STATE(Algorithm::AddressTableState, value, op->index, instruction);
}

void Algorithm::decodeState(const State *state)
{
    if(r_docnew->isInstructionCached(state->address))
        return;

    PIMPL_P(Algorithm);
    CachedInstruction instruction = r_docnew->cacheInstruction(state->address);
    size_t status = p->disassemble(state->address, instruction);

    if(status == Algorithm::SKIP) return;
    r_docnew->instruction(instruction);
}

void Algorithm::jumpState(const State *state)
{
    int dir = BRANCH_DIRECTION(state->instruction, state->address);
    if(!dir) r_docnew->autoComment(state->instruction->address, "Infinite loop");

    r_docnew->branch(state->address, dir);
    DECODE_STATE(state->address);
}

void Algorithm::callState(const State *state) { r_docnew->function(state->address); }

void Algorithm::branchState(const State *state)
{
    CachedInstruction instruction = state->instruction;

    if(instruction->is(InstructionType::Call))
        FORWARD_STATE(Algorithm::CallState, state);
    else if(instruction->is(InstructionType::Jump))
        FORWARD_STATE(Algorithm::JumpState, state);
    else
    {
        r_ctx->problem("Invalid branch state for instruction " + instruction->mnemonic.quoted() +
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

    const Symbol* symbol = r_docnew->symbol(state->address);

    if(symbol && symbol->is(SymbolType::ImportNew)) // Don't dereference imports
        return;

    u64 value = 0;
    r_disasm->dereference(state->address, &value);
    r_docnew->pointer(state->address);

    if(instruction->is(InstructionType::Call)) r_docnew->function(value);
    else r_docnew->label(value);

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

        if(instruction->is(InstructionType::Call))
            r_docnew->autoComment(instruction->address, "Call Table with " + String::number(c) + " cases(s)");
        else if(instruction->is(InstructionType::Jump))
            r_docnew->autoComment(instruction->address, "Jump Table with " + String::number(c) + " cases(s)");
        else
        {
            r_docnew->autoComment(instruction->address, "Address Table with " + String::number(c) + " cases(s)");
            fwdstate = Algorithm::MemoryState;
        }

        SortedSet targets = r_disasm->getTargets(instruction->address);
        targets.each([&](const Variant& v) { FORWARD_STATE_VALUE(fwdstate, v.toU64(), state); });
        return;
    }

    const Operand* op = state->operand();

    if(op->typeIs(OperandType::Displacement)) FORWARD_STATE(Algorithm::PointerState, state);
    else if(op->typeIs(OperandType::Memory)) FORWARD_STATE(Algorithm::MemoryState, state);
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

    if(instruction->is(InstructionType::Branch) && state->operand()->isTarget())
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

    r_docnew->pointer(state->address);
    r_disasm->checkLocation(state->address, value); // Create Symbol + XRefs

    const Symbol* symbol = r_docnew->symbol(value);
    if(!symbol) return;

    if(symbol->typeIs(SymbolType::StringNew))
    {
        if(symbol->hasFlag(SymbolFlags::WideString))
            r_docnew->autoComment(state->instruction->address, "=> WIDE STRING: " + r_disasm->readWString(value).quoted());
        else
            r_docnew->autoComment(state->instruction->address, "=> STRING: " + r_disasm->readString(value).quoted());
    }
    else if(symbol->isImport())
        r_docnew->autoComment(state->instruction->address, "=> IMPORT: " + symbol->name);
    else if(symbol->isExport())
        r_docnew->autoComment(state->instruction->address, "=> EXPORT: " + symbol->name);
    else
        return;

    r_disasm->pushReference(value, state->instruction->address);
}

void Algorithm::immediateState(const State *state)
{
    CachedInstruction instruction = state->instruction;

    if(instruction->is(InstructionType::Branch) && state->operand()->isTarget())
        FORWARD_STATE(Algorithm::BranchState, state);
    else
        r_disasm->checkLocation(instruction->address, state->address); // Create Symbol + XRefs
}

} // namespace REDasm
