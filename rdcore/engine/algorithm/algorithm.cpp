#include "algorithm.h"
#include "../../document/document.h"
#include "../../support/utils.h"
#include "../../support/sugar.h"
#include "../../disassembler.h"
#include "../../context.h"
#include <rdapi/disassembler.h>
#include <cassert>

Algorithm::Algorithm(Disassembler* disassembler): StateMachine(disassembler) { }

bool Algorithm::decodeInstruction(address_t address, RDInstruction** instruction)
{
    if(!m_document->isInstructionCached(address)) this->decodeAddress(address);
    return m_document->lockInstruction(address, instruction);
}

void Algorithm::handleOperand(const RDInstruction* instruction, const RDOperand* operand)
{
    switch(operand->type)
    {
        case OperandType_Void: break;
        case OperandType_Register: break;
        case OperandType_Memory: this->memoryState(instruction, operand->address); break;
        case OperandType_Constant: this->constantState(instruction, operand->u_value); break;
        case OperandType_Immediate: this->immediateState(instruction, operand->u_value); break;

        case OperandType_Displacement:
            if(!Sugar::displacementCanBeAddress(operand)) return;
            this->memoryState(instruction, operand->displacement);
            break;

        default: assert(false);
    }
}

void Algorithm::enqueueAddress(const RDInstruction* instruction, address_t address)
{
    switch(instruction->type)
    {
        case InstructionType_Call: this->callState(instruction, address); break;
        case InstructionType_Jump: this->jumpState(instruction, address); break;
        default: m_disassembler->pushReference(address, instruction->address); break;
    }

    this->schedule(address);
}

size_t Algorithm::decode(address_t address, RDInstruction* instruction)
{
    if(!this->canBeDisassembled(address)) return Algorithm::SKIP;
    instruction->address = address;

    std::unique_ptr<BufferView> view(rd_ldr->view(address));
    if(!view || view->empty()) return Algorithm::SKIP;
    return m_disassembler->decode(view.get(), instruction) ? Algorithm::OK : Algorithm::FAIL;
}

bool Algorithm::canBeDisassembled(address_t address) const
{
    if(m_document->isInstructionCached(address)) return false;

    if(!m_document->segment(address, &m_currentsegment)|| !HAS_FLAG(&m_currentsegment, SegmentFlags_Code))
        return false;

    RDBlock b;
    assert(m_document->block(address, &b));

    if(IS_TYPE(&b, BlockType_Code)) return false;

    if(IS_TYPE(&b, BlockType_Data))
    {
        RDSymbol symbol;
        assert(m_document->symbol(b.start, &symbol));

        switch(symbol.type)
        {
            case SymbolType_Label:
            case SymbolType_Function: return true;
            default: break;
        }

        return HAS_FLAG(&symbol, SymbolFlags_Weak);
    }

    return true;
}

void Algorithm::decodeAddress(address_t address)
{
    RDInstruction instruction{ };
    size_t result = this->decode(address, &instruction);

    switch(result)
    {
        case Algorithm::OK: m_disassembler->emulate(&instruction); break;

        case Algorithm::FAIL:
            this->invalidInstruction(&instruction);
            this->decodeFailed(&instruction);
            break;

        default: return;
    }

    m_document->instruction(&instruction);
}

void Algorithm::decodeFailed(const RDInstruction* instruction)
{
    rd_ctx->problem("Invalid instruction @ " + Utils::hex(instruction->address));

    if(!instruction->size) return;
    this->enqueue(Sugar::nextAddress(instruction));
}

void Algorithm::branchMemoryState(const RDInstruction* instruction, address_t value)
{
    m_disassembler->pushTarget(value, instruction->address);

    RDSymbol symbol;
    if(m_document->symbol(value, &symbol) && IS_TYPE(&symbol, SymbolType_Import)) return; // Don't dereference imports

    RDLocation loc = m_disassembler->dereference(value);
    if(!loc.valid) return;

    m_document->pointer(value, SymbolType_Data, std::string());

    if(instruction->type == InstructionType_Call) m_document->function(loc.address, std::string());
    else m_document->label(loc.address);

    m_disassembler->pushReference(loc.address, value);
    this->enqueue(loc.address);
}

void Algorithm::pointerState(const RDInstruction* instruction, address_t value)
{
    RDLocation loc = m_disassembler->dereference(value);

    if(!loc.valid)
    {
        this->immediateState(instruction, value);
        return;
    }

    m_document->pointer(value, SymbolType_Data, std::string());
    m_disassembler->markLocation(value, loc.value);

    RDSymbol symbol;
    if(!m_document->symbol(loc.value, &symbol)) return;

    if(IS_TYPE(&symbol, SymbolType_String))
    {
        if(HAS_FLAG(&symbol, SymbolFlags_WideString)) m_document->autoComment(instruction->address, "=> WIDE STRING: " + Utils::quoted(m_disassembler->readWString(loc.value)));
        else m_document->autoComment(instruction->address, "=> STRING: " + Utils::quoted(m_disassembler->readWString(loc.value)));
    }
    else if(HAS_FLAG(&symbol, SymbolType_Import))
        m_document->autoComment(instruction->address, std::string("=> IMPORT: ") + m_document->name(symbol.address));
    else if(HAS_FLAG(&symbol, SymbolFlags_Export))
        m_document->autoComment(instruction->address, std::string("=> EXPORT: ") + m_document->name(symbol.address));
    else
        return;

    m_disassembler->pushReference(loc.value, instruction->address);
}

void Algorithm::invalidInstruction(RDInstruction* instruction) const
{
    if(!instruction->size) instruction->size = 1; // Invalid instructions uses at least 1 byte

    instruction->type = InstructionType_Invalid;
    instruction->mnemonic[0] = 'd';
    instruction->mnemonic[1] = 'b';
    instruction->mnemonic[2] = '\0';
}

void Algorithm::jumpState(const RDInstruction* instruction, address_t value)
{
    int dir = Sugar::branchDirection(instruction, value);
    if(!dir) m_document->autoComment(instruction->address, "Infinite loop");

    m_document->branch(value, dir);
    m_disassembler->pushTarget(value, instruction->address);
}

void Algorithm::callState(const RDInstruction* instruction, address_t value)
{
    m_document->function(value, std::string());
    m_disassembler->pushTarget(value, instruction->address);
}

//void Algorithm::addressTableState(const State* state)
//{
    // const RDInstruction* instruction = state->instruction;
    // size_t c = m_disassembler->markTable(instruction, state->address);
    // if(c == RD_NPOS) return;

    // if(c > 1)
    // {
    //     m_disassembler->pushReference(state->address, instruction->address);
    //     state_t fwdstate = Algorithm::State_Branch;

    //     switch(instruction->type)
    //     {
    //         case InstructionType_Call:
    //             m_document->autoComment(instruction->address, "Call Table with " + Utils::number(c) + " cases(s)");
    //             break;

    //         case InstructionType_Jump:
    //             m_document->autoComment(instruction->address, "Jump Table with " + Utils::number(c) + " cases(s)");
    //             break;

    //         default:
    //             m_document->autoComment(instruction->address, "Address Table with " + Utils::number(c) + " cases(s)");
    //             fwdstate = Algorithm::State_Memory;
    //             break;
    //     }

    //     const address_t* targets = nullptr;
    //     size_t c = m_disassembler->getTargets(instruction->address, &targets);
    //     for(size_t i = 0; i < c; i++) FORWARD_STATE_VALUE(fwdstate, targets[i], state);
    //     return;
    // }

    // const RDOperand* op = &instruction->operands[state->opindex];

    // switch(op->type)
    // {
    //     case OperandType_Displacement: FORWARD_STATE(Algorithm::State_Pointer, state); break;
    //     case OperandType_Memory: FORWARD_STATE(Algorithm::State_Memory, state); break;
    //     default: FORWARD_STATE(Algorithm::State_Immediate, state); break;
    // }
//}

void Algorithm::memoryState(const RDInstruction* instruction, address_t value)
{
   RDLocation loc = m_disassembler->dereference(value);

   if(!loc.valid)
   {
       this->immediateState(instruction, value);
       return;
   }

   m_disassembler->pushReference(value, instruction->address);

   if(Sugar::isBranch(instruction)) this->branchMemoryState(instruction, value);
   else this->pointerState(instruction, value);
}

void Algorithm::immediateState(const RDInstruction* instruction, address_t value) { m_disassembler->markLocation(instruction->address, value); }

void Algorithm::constantState(const RDInstruction* instruction, address_t value)
{
    if(!Sugar::isCharacter(value)) return;
    std::string charinfo = Utils::hex(value, 8, true) + "=" + Utils::quotedSingle(std::string(1, static_cast<char>(value)));
    m_document->autoComment(instruction->address, charinfo);
}
