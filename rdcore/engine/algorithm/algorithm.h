#pragma once

#include <rdapi/disassembler.h>
#include "../../support/safe_ptr.h"
#include "statemachine.h"

class Algorithm: public StateMachine
{
    private:
        enum { OK, SKIP, FAIL };

    public:
        Algorithm(Disassembler* disassembler);
        bool decodeInstruction(rd_address address, RDInstruction** instruction);
        void checkOperands(const RDInstruction* instruction);
        void checkOperand(const RDInstruction* instruction, const RDOperand* operand);
        void enqueueAddress(const RDInstruction* instruction, rd_address address);

    protected:
        void decodeAddress(rd_address address) override;

    private:
        size_t decode(rd_address address, RDInstruction* instruction, RDBlock* block);
        bool canBeDisassembled(rd_address address, RDBlock* block) const;
        void decodeFailed(RDInstruction* instruction);
        void invalidInstruction(RDInstruction* instruction) const;

    private: // Private States
        void branchMemoryState(const RDInstruction* instruction, rd_address value);

    private:
        void jumpState(const RDInstruction* instruction, rd_address value);
        void callState(const RDInstruction* instruction, rd_address value);
        void memoryState(const RDInstruction* instruction, rd_address value);
        void immediateState(const RDInstruction* instruction, rd_address value);
        void constantState(const RDInstruction* instruction, rd_address value);

    private:
        mutable RDSegment m_currentsegment{ };
};

typedef safe_ptr<Algorithm> SafeAlgorithm;
