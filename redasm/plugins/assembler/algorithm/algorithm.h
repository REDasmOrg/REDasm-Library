#ifndef ASSEMBLERALGORITHM_H
#define ASSEMBLERALGORITHM_H

#include <stack>
#include <set>
#include "../../../disassembler/disassemblerapi.h"
#include "../../../redasm.h"
#include "../../../analyzer/analyzer.h"
#include "statemachine.h"

namespace REDasm {

class AssemblerAlgorithm: public StateMachine
{
    DEFINE_STATES(DecodeState,
                  JumpState, CallState, BranchState, BranchMemoryState,
                  AddressTableState, MemoryState, PointerState, ImmediateState)

    public:
        enum: u32 { OK, SKIP, FAIL };

    private:
        typedef std::unordered_set<address_t> DecodedAddresses;

    protected:
        AssemblerAlgorithm(DisassemblerAPI* disassembler, AssemblerPlugin* assembler);

    public:
        AssemblerAlgorithm();
        u32 disassembleInstruction(address_t address, const InstructionPtr& instruction);
        void enqueue(address_t address);
        void analyze();

    protected:
        virtual void validateTarget(const InstructionPtr& instruction) const;
        virtual bool validateState(const State& state) const;
        virtual void onNewState(const State *state) const;
        virtual void onDecoded(const InstructionPtr& instruction);
        virtual void onDecodeFailed(const InstructionPtr& instruction);
        virtual void onDecodedOperand(const Operand& op, const InstructionPtr& instruction);
        virtual void onEmulatedOperand(const Operand& op, const InstructionPtr& instruction, u64 value);

    protected:
        virtual void decodeState(const State *state);
        virtual void jumpState(const State *state);
        virtual void callState(const State *state);
        virtual void branchState(const State *state);
        virtual void branchMemoryState(const State *state);
        virtual void addressTableState(const State *state);
        virtual void memoryState(const State* state);
        virtual void pointerState(const State* state);
        virtual void immediateState(const State* state);

    private:
        bool canBeDisassembled(address_t address);
        void createInvalidInstruction(const InstructionPtr& instruction);
        u32 disassemble(address_t address, const InstructionPtr& instruction);
        void emulateOperand(const Operand& op, const InstructionPtr& instruction);
        void emulate(const InstructionPtr& instruction);

    protected:
        std::unique_ptr<Emulator> m_emulator;
        ListingDocument m_document;
        DisassemblerAPI* m_disassembler;
        AssemblerPlugin* m_assembler;
        FormatPlugin* m_format;

    private:
        DecodedAddresses m_disassembled;
        std::unique_ptr<Analyzer> m_analyzer;
        const Segment* m_currentsegment;
        bool m_analyzed;
};

} // namespace REDasm

#endif // ASSEMBLERALGORITHM_H
