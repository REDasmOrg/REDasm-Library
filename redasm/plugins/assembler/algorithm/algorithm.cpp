#include "algorithm.h"
#include "../../../plugins/loader.h"
#include <thread>

#define INVALID_MNEMONIC "db"
#define DECODE_STATE(address) ENQUEUE_STATE(AssemblerAlgorithm::DecodeState, address, -1, nullptr)

namespace REDasm {

AssemblerAlgorithm::AssemblerAlgorithm(): StateMachine(), m_disassembler(nullptr), m_assembler(nullptr) { }

AssemblerAlgorithm::AssemblerAlgorithm(DisassemblerAPI *disassembler, AssemblerPlugin *assembler): StateMachine(), m_document(disassembler->document()), m_disassembler(disassembler), m_assembler(assembler), m_currentsegment(nullptr), m_analyzed(0)
{
    m_loader = m_disassembler->loader();

    if(assembler->hasFlag(AssemblerFlags::CanEmulate))
        m_emulator = std::unique_ptr<Emulator>(assembler->createEmulator(disassembler));

    REGISTER_STATE(AssemblerAlgorithm::DecodeState, &AssemblerAlgorithm::decodeState);
    REGISTER_STATE(AssemblerAlgorithm::JumpState, &AssemblerAlgorithm::jumpState);
    REGISTER_STATE(AssemblerAlgorithm::CallState, &AssemblerAlgorithm::callState);
    REGISTER_STATE(AssemblerAlgorithm::BranchState, &AssemblerAlgorithm::branchState);
    REGISTER_STATE(AssemblerAlgorithm::BranchMemoryState, &AssemblerAlgorithm::branchMemoryState);
    REGISTER_STATE(AssemblerAlgorithm::AddressTableState, &AssemblerAlgorithm::addressTableState);
    REGISTER_STATE(AssemblerAlgorithm::MemoryState, &AssemblerAlgorithm::memoryState);
    REGISTER_STATE(AssemblerAlgorithm::PointerState, &AssemblerAlgorithm::pointerState);
    REGISTER_STATE(AssemblerAlgorithm::ImmediateState, &AssemblerAlgorithm::immediateState);
}

void AssemblerAlgorithm::enqueue(address_t address) { DECODE_STATE(address); }

void AssemblerAlgorithm::analyze()
{
    if(m_analyzed)
    {
        REDasm::status("Analyzing (Fast)...");
        m_analyzer->analyzeFast();
        m_document->moveToEP();
        return;
    }

    m_analyzed = true;
    LoaderPlugin* loader = m_disassembler->loader();
    m_analyzer.reset(loader->createAnalyzer(m_disassembler));

    REDasm::status("Analyzing...");
    m_analyzer->analyze();
    m_document->moveToEP();

    // Trigger a Fast Analysis when post disassembling is completed
    EVENT_CONNECT(m_disassembler, busyChanged, this, [&]() {
        if(m_disassembler->busy())
            return;

        this->analyze();
    });
}

void AssemblerAlgorithm::loadTargets(const InstructionPtr &instruction)
{
    for(address_t target : instruction->meta.targets) // Get precalculated targets
        m_disassembler->pushTarget(target, instruction->address);
}

void AssemblerAlgorithm::validateTarget(const InstructionPtr &instruction) const
{
    if(m_disassembler->getTargetsCount(instruction->address))
        return;

    const Operand* op = instruction->target();

    if(op && !op->isNumeric())
        return;

    REDasm::log("No targets found for " + REDasm::quoted(instruction->mnemonic) + " @ " + REDasm::hex(instruction->address));
}

bool AssemblerAlgorithm::validateState(const State &state) const
{
    if(!StateMachine::validateState(state))
        return false;

    return m_document->segment(state.address);
}

void AssemblerAlgorithm::onNewState(const State* state) const
{
    REDasm::statusProgress("Analyzing @ " + REDasm::hex(state->address, m_assembler->bits()) +
                           " >> " + state->name, this->pending());
}

u32 AssemblerAlgorithm::disassembleInstruction(address_t address, const InstructionPtr& instruction)
{
    if(!this->canBeDisassembled(address))
        return AssemblerAlgorithm::SKIP;

    instruction->address = address;

    BufferView view = m_loader->view(address);
    return m_assembler->decode(view, instruction) ? AssemblerAlgorithm::OK : AssemblerAlgorithm::FAIL;
}

void AssemblerAlgorithm::onDecoded(const InstructionPtr &instruction)
{
    if(instruction->is(InstructionTypes::Branch))
    {
        this->loadTargets(instruction);
        this->validateTarget(instruction);
    }

    for(const Operand& op : instruction->operands)
    {
        if(!op.isNumeric() || op.displacementIsDynamic())
        {
            if(m_emulator && !m_emulator->hasError())
                this->emulateOperand(&op, instruction);

            if(!op.is(OperandTypes::Displacement)) // Try static displacement analysis
                continue;
        }

        if(op.is(OperandTypes::Displacement))
        {
            if(op.displacementIsDynamic())
                EXECUTE_STATE(AssemblerAlgorithm::AddressTableState, op.disp.displacement, op.index, instruction);
            else if(op.displacementCanBeAddress())
                EXECUTE_STATE(AssemblerAlgorithm::MemoryState, op.disp.displacement, op.index, instruction);
        }
        else if(op.is(OperandTypes::Memory))
            EXECUTE_STATE(AssemblerAlgorithm::MemoryState, op.u_value, op.index, instruction);
        else if(op.is(OperandTypes::Immediate))
            EXECUTE_STATE(AssemblerAlgorithm::ImmediateState, op.u_value, op.index, instruction);

        this->onDecodedOperand(&op, instruction);
    }
}

void AssemblerAlgorithm::onDecodeFailed(const InstructionPtr &instruction)
{
    REDasm::log("Invalid instruction @ " + REDasm::hex(instruction->address));

    if(!instruction->size)
        return;

    this->enqueue(instruction->endAddress());
}

void AssemblerAlgorithm::onDecodedOperand(const Operand *op, const InstructionPtr &instruction)
{
    if(!op->isCharacter())
        return;

    std::string charinfo = REDasm::hex(op->u_value, 8, true) + "=" + REDasm::quoted_s(std::string(1, static_cast<char>(op->u_value)));
    m_document->autoComment(instruction->address, charinfo);
}

void AssemblerAlgorithm::onEmulatedOperand(const Operand *op, const InstructionPtr &instruction, u64 value)
{
    Segment* segment = m_document->segment(value);

    if(!segment || segment->isPureCode()) // Don't flood "Pure-Code" segments with symbols
        return;

    EXECUTE_STATE(AssemblerAlgorithm::AddressTableState, value, op->index, instruction);
}

void AssemblerAlgorithm::decodeState(const State *state)
{
    InstructionPtr instruction = std::make_shared<Instruction>();
    u32 status = this->disassemble(state->address, instruction);

    if(status == AssemblerAlgorithm::SKIP)
        return;

    m_document->instruction(instruction);
}

void AssemblerAlgorithm::jumpState(const State *state)
{
    s64 dir = BRANCH_DIRECTION(state->instruction, state->address);

    if(!dir)
        m_document->autoComment(state->instruction->address, "Infinite loop");

    m_document->branch(state->address, dir);
    DECODE_STATE(state->address);
}

void AssemblerAlgorithm::callState(const State *state) { m_document->symbol(state->address, SymbolTypes::Function); }

void AssemblerAlgorithm::branchState(const State *state)
{
    InstructionPtr instruction = state->instruction;

    if(instruction->is(InstructionTypes::Call))
        FORWARD_STATE(AssemblerAlgorithm::CallState, state);
    else if(instruction->is(InstructionTypes::Jump))
        FORWARD_STATE(AssemblerAlgorithm::JumpState, state);
    else
    {
        REDasm::log("Invalid branch state for instruction " + REDasm::quoted(instruction->mnemonic) +
                    " @ " + REDasm::hex(instruction->address, m_assembler->bits()));
        return;
    }

    m_disassembler->pushReference(state->address, instruction->address);
    m_disassembler->pushTarget(state->address, instruction->address);
}

void AssemblerAlgorithm::branchMemoryState(const State *state)
{
    InstructionPtr instruction = state->instruction;
    m_disassembler->pushTarget(state->address, instruction->address);

    Symbol* symbol = m_document->symbol(state->address);

    if(symbol && symbol->isImport()) // Don't dereference imports
        return;

    u64 value = 0;
    m_disassembler->dereference(state->address, &value);
    m_document->symbol(state->address, SymbolTypes::Data | SymbolTypes::Pointer);

    if(instruction->is(InstructionTypes::Call))
        m_document->symbol(value, SymbolTypes::Function);
    else
        m_document->symbol(value, SymbolTypes::Code);

    m_disassembler->pushReference(value, state->address);
}

void AssemblerAlgorithm::addressTableState(const State *state)
{
    InstructionPtr instruction = state->instruction;
    s64 c = m_disassembler->checkAddressTable(instruction, state->address);

    if(c < 0)
        return;

    if(c > 1)
    {
        m_disassembler->pushReference(state->address, instruction->address);
        state_t fwdstate = AssemblerAlgorithm::BranchState;

        if(instruction->is(InstructionTypes::Call))
            m_document->autoComment(instruction->address, "Call Table with " + std::to_string(c) + " cases(s)");
        else if(instruction->is(InstructionTypes::Jump))
            m_document->autoComment(instruction->address, "Jump Table with " + std::to_string(c) + " cases(s)");
        else
        {
            m_document->autoComment(instruction->address, "Address Table with " + std::to_string(c) + " cases(s)");
            fwdstate = AssemblerAlgorithm::MemoryState;
        }

        ReferenceSet targets = m_disassembler->getTargets(instruction->address);

        for(address_t target : targets)
            FORWARD_STATE_VALUE(fwdstate, target, state);

        return;
    }

    const Operand* op = state->operand();

    if(op->is(OperandTypes::Displacement))
        FORWARD_STATE(AssemblerAlgorithm::PointerState, state);
    else if(op->is(OperandTypes::Memory))
        FORWARD_STATE(AssemblerAlgorithm::MemoryState, state);
    else
        FORWARD_STATE(AssemblerAlgorithm::ImmediateState, state);
}

void AssemblerAlgorithm::memoryState(const State *state)
{
    u64 value = 0;

    if(!m_disassembler->dereference(state->address, &value))
    {
        FORWARD_STATE(AssemblerAlgorithm::ImmediateState, state);
        return;
    }

    InstructionPtr instruction = state->instruction;
    m_disassembler->pushReference(state->address, instruction->address);

    if(instruction->is(InstructionTypes::Branch) && state->operand()->isTarget())
        FORWARD_STATE(AssemblerAlgorithm::BranchMemoryState, state);
    else
        FORWARD_STATE(AssemblerAlgorithm::PointerState, state);
}

void AssemblerAlgorithm::pointerState(const State *state)
{
    u64 value = 0;

    if(!m_disassembler->dereference(state->address, &value))
    {
        FORWARD_STATE(AssemblerAlgorithm::ImmediateState, state);
        return;
    }

    m_document->symbol(state->address, SymbolTypes::Data | SymbolTypes::Pointer);
    m_disassembler->checkLocation(state->address, value); // Create Symbol + XRefs
}

void AssemblerAlgorithm::immediateState(const State *state)
{
    InstructionPtr instruction = state->instruction;

    if(instruction->is(InstructionTypes::Branch) && state->operand()->isTarget())
        FORWARD_STATE(AssemblerAlgorithm::BranchState, state);
    else
        m_disassembler->checkLocation(instruction->address, state->address); // Create Symbol + XRefs
}

bool AssemblerAlgorithm::canBeDisassembled(address_t address)
{
    BufferView view = m_loader->view(address);

    if(view.eob())
        return false;

    if(!m_currentsegment || !m_currentsegment->contains(address))
        m_currentsegment = m_document->segment(address);

    if(!m_currentsegment || !m_currentsegment->is(SegmentTypes::Code))
        return false;

    if(!m_loader->offset(address).valid)
        return false;

    Symbol* symbol = m_document->symbol(address);

    if(symbol && !symbol->is(SymbolTypes::Code))
        return false;

    return true;
}

void AssemblerAlgorithm::createInvalidInstruction(const InstructionPtr &instruction)
{
    if(!instruction->size)
        instruction->size = 1; // Invalid instruction uses at least 1 byte

    instruction->type = InstructionTypes::Invalid;
    instruction->mnemonic = INVALID_MNEMONIC;
}

u32 AssemblerAlgorithm::disassemble(address_t address, const InstructionPtr &instruction)
{
    auto it = m_disassembled.find(address);

    if(it != m_disassembled.end())
        return AssemblerAlgorithm::SKIP;

    m_disassembled.insert(address);
    u32 result = this->disassembleInstruction(address, instruction);

    if(result == AssemblerAlgorithm::FAIL)
    {
        this->createInvalidInstruction(instruction);
        this->onDecodeFailed(instruction);
    }
    else
    {
        this->emulate(instruction);
        this->onDecoded(instruction);
    }

    return result;
}

void AssemblerAlgorithm::emulateOperand(const Operand *op, const InstructionPtr &instruction)
{
    u64 value = 0;

    if(op->is(OperandTypes::Register))
    {
        if(!m_emulator->read(op, &value))
            return;
    }
    else if(op->is(OperandTypes::Displacement))
    {
        if(!m_emulator->displacement(op, &value))
            return;
    }
    else
        return;

    this->onEmulatedOperand(op, instruction, value);
}

void AssemblerAlgorithm::emulate(const InstructionPtr &instruction)
{
    if(!m_emulator)
        return;

    m_emulator->emulate(instruction);
}


} // namespace REDasm
