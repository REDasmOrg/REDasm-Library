#pragma once

#define ENQUEUE_STATE(id, value, index, instruction)  this->enqueueState({ #id, id, {u64(value)}, index, instruction })
#define DECODE_STATE(address)                         ENQUEUE_STATE(Algorithm::State_Decode, address, RD_NPOS, nullptr)

#include "statemachine.h"

class Algorithm: public StateMachine
{
    private:
        enum { OK, SKIP, FAIL };

    public:
        Algorithm();
        virtual ~Algorithm() = default;
        const RDInstruction* decodeInstruction(address_t address);
        void enqueue(address_t address);

    private:
        size_t decode(address_t address, RDInstruction* instruction);
        void decode(address_t address);
        void validateTarget(const RDInstruction* instruction) const;
        void invalidInstruction(RDInstruction* instruction) const;
        bool canBeDisassembled(address_t address);

    protected:
        virtual void onDecodedOperand(const RDOperand* op, const RDInstruction* instruction);
        virtual void onDecodeFailed(const RDInstruction* instruction);
        virtual void onDecoded(const RDInstruction* instruction);

    protected:
        virtual void decodeState(const RDState *state);
        virtual void jumpState(const RDState *state);
        virtual void callState(const RDState *state);
        virtual void branchState(const RDState *state);
        virtual void branchMemoryState(const RDState *state);
        virtual void addressTableState(const RDState *state);
        virtual void memoryState(const RDState* state);
        virtual void pointerState(const RDState* state);
        virtual void immediateState(const RDState* state);

    private:
        RDSegment m_currentsegment{};
};

