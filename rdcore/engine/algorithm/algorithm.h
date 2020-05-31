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
        bool decodeInstruction(address_t address, RDInstruction** instruction);
        void handleOperand(const RDInstruction* instruction, const RDOperand* operand);
        void enqueueAddress(const RDInstruction* instruction, address_t address);

    protected:
        void decodeAddress(address_t address) override;

    private:
        size_t decode(address_t address, RDInstruction* instruction);
        bool canBeDisassembled(address_t address) const;
        void decodeFailed(RDInstruction* instruction);
        void invalidInstruction(RDInstruction* instruction) const;

    private: // Private States
        void branchMemoryState(const RDInstruction* instruction, address_t value);

    private:
        void jumpState(const RDInstruction* instruction, address_t value);
        void callState(const RDInstruction* instruction, address_t value);
        //void addressTableState(const State *state);
        void memoryState(const RDInstruction* instruction, address_t value);
        void immediateState(const RDInstruction* instruction, address_t value);
        void constantState(const RDInstruction* instruction, address_t value);

    private:
        mutable RDSegment m_currentsegment{ };
};

typedef safe_ptr<Algorithm> SafeAlgorithm;
