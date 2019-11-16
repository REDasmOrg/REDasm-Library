#include "algorithm_impl.h"
#include <redasm/disassembler/disassembler.h>
#include <redasm/plugins/assembler/assembler.h>
#include <redasm/plugins/loader/loader.h>
#include <redasm/support/utils.h>
#include <redasm/context.h>
#include <algorithm>

#define INVALID_MNEMONIC "db"
#define REGISTER_STATE_PRIVATE(id, cb) m_states[id] = std::bind(cb, q, std::placeholders::_1)

namespace REDasm {

AlgorithmImpl::AlgorithmImpl(Algorithm *algorithm): StateMachine(), m_pimpl_q(algorithm), m_currentsegment(nullptr), m_analyzed(false)
{
    m_analyzer = nullptr;

    PIMPL_Q(Algorithm);

    REGISTER_STATE_PRIVATE(Algorithm::DecodeState, &Algorithm::decodeState);
    REGISTER_STATE_PRIVATE(Algorithm::JumpState, &Algorithm::jumpState);
    REGISTER_STATE_PRIVATE(Algorithm::CallState, &Algorithm::callState);
    REGISTER_STATE_PRIVATE(Algorithm::BranchState, &Algorithm::branchState);
    REGISTER_STATE_PRIVATE(Algorithm::BranchMemoryState, &Algorithm::branchMemoryState);
    REGISTER_STATE_PRIVATE(Algorithm::AddressTableState, &Algorithm::addressTableState);
    REGISTER_STATE_PRIVATE(Algorithm::MemoryState, &Algorithm::memoryState);
    REGISTER_STATE_PRIVATE(Algorithm::PointerState, &Algorithm::pointerState);
    REGISTER_STATE_PRIVATE(Algorithm::ImmediateState, &Algorithm::immediateState);
}

CachedInstruction AlgorithmImpl::decodeInstruction(address_t address)
{
    if(r_docnew->isInstructionCached(address)) return r_docnew->instruction(address);
    return this->decode(address);
}

size_t AlgorithmImpl::decode(address_t address, const CachedInstruction& instruction)
{
    if(!this->canBeDisassembled(address)) return Algorithm::SKIP;
    instruction->address = address;

    BufferView view = r_ldr->view(address);
    if(view.eob()) return Algorithm::SKIP;
    return r_asm->decode(view, instruction.get()) ? Algorithm::OK : Algorithm::FAIL;
}

CachedInstruction AlgorithmImpl::decode(address_t address)
{
    PIMPL_Q(Algorithm);
    CachedInstruction instruction = r_docnew->allocateInstruction();
    size_t result = this->decode(address, instruction);

    if(result == Algorithm::FAIL)
    {
        this->createInvalidInstruction(instruction);
        q->onDecodeFailed(instruction);
    }
    else if(result == Algorithm::OK) q->onDecoded(instruction);
    else
    {
        instruction.invalidate();
        return CachedInstruction();
    }

    r_docnew->instruction(instruction); // Store instruction in Document
    return instruction;
}

void AlgorithmImpl::enqueue(address_t address) { DECODE_STATE(address); }

void AlgorithmImpl::loadTargets(const CachedInstruction& instruction)
{
    for(size_t i = 0; i < instruction->targetscount; i++)
        r_disasm->pushTarget(instruction->targets[i], instruction->address);
}

bool AlgorithmImpl::validateState(const State &state) const
{
    if(!StateMachine::validateState(state))
        return false;

    return r_docnew->segment(state.address);
}

void AlgorithmImpl::onNewState(const State *state) const
{
    r_ctx->statusProgress("Analyzing @ " + String::hex(state->address, r_asm->bits()) +
                           " >> " + state->name, this->pending());
}

void AlgorithmImpl::validateTarget(const CachedInstruction& instruction) const
{
    if(r_disasm->getTargetsCount(instruction->address))
        return;

    const Operand* op = instruction->target();
    if(op && !Operand::isNumeric(op)) return;

    r_ctx->problem("No targets found for " + instruction->mnemonic().quoted() + " @ " + String::hex(instruction->address));
}

bool AlgorithmImpl::canBeDisassembled(address_t address)
{
    if(r_docnew->isInstructionCached(address)) return false;

    if(!m_currentsegment || !m_currentsegment->contains(address))
        m_currentsegment = r_docnew->segment(address);

    if(!m_currentsegment || !m_currentsegment->is(SegmentType::Code))
        return false;

    const BlockItem* bi = r_docnew->block(address);
    if(bi->typeIs(BlockItemType::Code)) return false;

    if(bi->typeIs(BlockItemType::Data))
    {
        const Symbol* symbol = r_docnew->symbol(bi->start);
        assert(symbol);
        return symbol->isFunction() || symbol->isWeak(); // It's allowed to disassemble above a Function
    }

    return true;
}

void AlgorithmImpl::createInvalidInstruction(const CachedInstruction& instruction)
{
    if(!instruction->size)
        instruction->size =1; // Invalid instruction uses at least 1 byte

    instruction->type = InstructionType::Invalid;
    instruction->mnemonic(INVALID_MNEMONIC);
}

} // namespace REDasm
