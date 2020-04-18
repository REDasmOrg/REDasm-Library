#pragma once

#include <array>
#include <forward_list>
#include <functional>
#include <rdapi/disassembler.h>

typedef std::function<void(const RDState*)> StateCallback;

class StateMachine
{
    protected:
        enum: size_t {
            State_Decode,
            State_Jump, State_Call,
            State_Branch, State_BranchMemory,
            State_AddressTable, State_Memory, State_Pointer, State_Immediate,
            State_Length
        };

    public:
        StateMachine() = default;
        virtual ~StateMachine() = default;
        size_t pending() const;
        bool hasNext() const;
        void next();

    protected:
        void executeState(const RDState* state);
        void executeState(const RDState& state);
        void enqueueState(const RDState& state);

    private:
        bool validateState(const RDState* state) const;
        void onNewState(const RDState* state) const;
        bool getNext(RDState* state);

    protected:
        std::array<StateCallback, State_Length> m_states;

    private:
        std::forward_list<RDState> m_pending;
        size_t m_count{0};
};

