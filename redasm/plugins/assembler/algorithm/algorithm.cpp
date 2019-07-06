#include "algorithm.h"
#include <impl/plugins/assembler/algorithm/algorithm_impl.h>
#include "../../../plugins/assembler/assembler.h"
#include "../../../disassembler/disassembler.h"
#include "../../../support/utils.h"
#include "../../../context.h"

namespace REDasm {

Algorithm::Algorithm(AlgorithmImpl *p): m_pimpl_p(p) { }
Algorithm::Algorithm(Disassembler *disassembler): m_pimpl_p(new AlgorithmImpl(this, disassembler)) { }
Disassembler *Algorithm::disassembler() const { PIMPL_P(const Algorithm); return p->m_disassembler; }
safe_ptr<ListingDocumentType> &Algorithm::document() const { PIMPL_P(const Algorithm); return p->m_disassembler->document(); }
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

            if(!op->is(OperandType::Displacement)) // Try static displacement analysis
                continue;
        }

        if(op->is(OperandType::Displacement))
        {
            if(op->displacementIsDynamic())
                EXECUTE_STATE(Algorithm::AddressTableState, op->disp.displacement, op->index, instruction);
            else if(op->displacementCanBeAddress())
                EXECUTE_STATE(Algorithm::MemoryState, op->disp.displacement, op->index, instruction);
        }
        else if(op->is(OperandType::Memory))
            EXECUTE_STATE(Algorithm::MemoryState, op->u_value, op->index, instruction);
        else if(op->is(OperandType::Immediate))
            EXECUTE_STATE(Algorithm::ImmediateState, op->u_value, op->index, instruction);

        this->onDecodedOperand(op, instruction);
    }
}

void Algorithm::onDecodeFailed(const CachedInstruction& instruction)
{
    r_ctx->problem("Invalid instruction @ " + String::hex(instruction->address));

    if(!instruction->size)
        return;

    this->enqueue(instruction->endAddress());
}

void Algorithm::onDecodedOperand(const Operand *op, const CachedInstruction &instruction)
{
    if(!op->isCharacter())
        return;

    PIMPL_P(Algorithm);
    String charinfo = String::hex(op->u_value, 8, true) + "=" + String(static_cast<char>(op->u_value)).quotedSingle();
    p->m_document->autoComment(instruction->address, charinfo);
}

void Algorithm::onEmulatedOperand(const Operand *op, const CachedInstruction &instruction, u64 value)
{
    PIMPL_P(Algorithm);
    Segment* segment = p->m_document->segment(value);

    if(!segment || segment->isPureCode()) // Don't flood "Pure-Code" segments with symbols
        return;

    EXECUTE_STATE(Algorithm::AddressTableState, value, op->index, instruction);
}

void Algorithm::decodeState(const State *state)
{
    if(this->document()->isInstructionCached(state->address))
        return;

    PIMPL_P(Algorithm);
    CachedInstruction instruction = this->document()->cacheInstruction(state->address);
    size_t status = p->disassemble(state->address, instruction);

    if(status == Algorithm::SKIP)
        return;

    p->m_document->instruction(instruction);
}

void Algorithm::jumpState(const State *state)
{
    PIMPL_P(Algorithm);
    int dir = BRANCH_DIRECTION(state->instruction, state->address);

    if(!dir)
        p->m_document->autoComment(state->instruction->address, "Infinite loop");

    p->m_document->branch(state->address, dir);
    DECODE_STATE(state->address);
}

void Algorithm::callState(const State *state) { PIMPL_P(Algorithm); p->m_document->symbol(state->address, SymbolType::Function); }

void Algorithm::branchState(const State *state)
{
    PIMPL_P(Algorithm);
    CachedInstruction instruction = state->instruction;

    if(instruction->is(InstructionType::Call))
        FORWARD_STATE(Algorithm::CallState, state);
    else if(instruction->is(InstructionType::Jump))
        FORWARD_STATE(Algorithm::JumpState, state);
    else
    {
        r_ctx->problem("Invalid branch state for instruction " + instruction->mnemonic.quoted() +
                       " @ " + String::hex(instruction->address, p->m_assembler->bits()));
        return;
    }

    p->m_disassembler->pushReference(state->address, instruction->address);
    p->m_disassembler->pushTarget(state->address, instruction->address);
}

void Algorithm::branchMemoryState(const State *state)
{
    PIMPL_P(Algorithm);
    CachedInstruction instruction = state->instruction;
    p->m_disassembler->pushTarget(state->address, instruction->address);

    Symbol* symbol = p->m_document->symbol(state->address);

    if(symbol && symbol->isImport()) // Don't dereference imports
        return;

    u64 value = 0;
    p->m_disassembler->dereference(state->address, &value);
    p->m_document->symbol(state->address, SymbolType::Data | SymbolType::Pointer);

    if(instruction->is(InstructionType::Call))
        p->m_document->symbol(value, SymbolType::Function);
    else
        p->m_document->symbol(value, SymbolType::Code);

    p->m_disassembler->pushReference(value, state->address);
}

void Algorithm::addressTableState(const State *state)
{
    PIMPL_P(Algorithm);
    CachedInstruction instruction = state->instruction;
    size_t c = p->m_disassembler->checkAddressTable(instruction, state->address);

    if(c == REDasm::npos)
        return;

    if(c > 1)
    {
        p->m_disassembler->pushReference(state->address, instruction->address);
        state_t fwdstate = Algorithm::BranchState;

        if(instruction->is(InstructionType::Call))
            p->m_document->autoComment(instruction->address, "Call Table with " + String::number(c) + " cases(s)");
        else if(instruction->is(InstructionType::Jump))
            p->m_document->autoComment(instruction->address, "Jump Table with " + String::number(c) + " cases(s)");
        else
        {
            p->m_document->autoComment(instruction->address, "Address Table with " + String::number(c) + " cases(s)");
            fwdstate = Algorithm::MemoryState;
        }

        ReferenceSet targets = p->m_disassembler->getTargets(instruction->address);

        for(address_t target : targets)
            FORWARD_STATE_VALUE(fwdstate, target, state);

        return;
    }

    const Operand* op = state->operand();

    if(op->is(OperandType::Displacement))
        FORWARD_STATE(Algorithm::PointerState, state);
    else if(op->is(OperandType::Memory))
        FORWARD_STATE(Algorithm::MemoryState, state);
    else
        FORWARD_STATE(Algorithm::ImmediateState, state);
}

void Algorithm::memoryState(const State *state)
{
    PIMPL_P(Algorithm);
    u64 value = 0;

    if(!p->m_disassembler->dereference(state->address, &value))
    {
        FORWARD_STATE(Algorithm::ImmediateState, state);
        return;
    }

    CachedInstruction instruction = state->instruction;
    p->m_disassembler->pushReference(state->address, instruction->address);

    if(instruction->is(InstructionType::Branch) && state->operand()->isTarget())
        FORWARD_STATE(Algorithm::BranchMemoryState, state);
    else
        FORWARD_STATE(Algorithm::PointerState, state);
}

void Algorithm::pointerState(const State *state)
{
    PIMPL_P(Algorithm);
    u64 value = 0;

    if(!p->m_disassembler->dereference(state->address, &value))
    {
        FORWARD_STATE(Algorithm::ImmediateState, state);
        return;
    }

    p->m_document->symbol(state->address, SymbolType::Data | SymbolType::Pointer);
    p->m_disassembler->checkLocation(state->address, value); // Create Symbol + XRefs
}

void Algorithm::immediateState(const State *state)
{
    PIMPL_P(Algorithm);
    CachedInstruction instruction = state->instruction;

    if(instruction->is(InstructionType::Branch) && state->operand()->isTarget())
        FORWARD_STATE(Algorithm::BranchState, state);
    else
        p->m_disassembler->checkLocation(instruction->address, state->address); // Create Symbol + XRefs
}

} // namespace REDasm
