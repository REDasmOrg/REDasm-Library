#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <unordered_map>
#include <unordered_set>
#include <forward_list>
#include <functional>
#include "../../../support/safe_ptr.h"
#include "../../../redasm.h"

#define DEFINE_STATES(...)                                   protected: enum: state_t { __VA_ARGS__ }; private:
#define REGISTER_STATE(id, cb)                               m_states[id] = std::bind(cb, this, std::placeholders::_1)
#define EXECUTE_STATE(id, value, index, instruction)         this->executeState({ #id, id, static_cast<u64>(value), index, instruction })
#define ENQUEUE_STATE(id, value, index, instruction)         this->enqueueState({ #id, id, static_cast<u64>(value), index, instruction })
#define FORWARD_STATE_VALUE(newid, value, state)             EXECUTE_STATE(newid, value, state->index, state->instruction)
#define FORWARD_STATE(newid, state)                          FORWARD_STATE_VALUE(newid, state->u_value, state)

namespace REDasm {

typedef u32 state_t;

struct State
{
    std::string name;
    state_t id;

    union {
        u64 u_value;
        s64 s_value;
        address_t address;
    };

    s64 index;
    InstructionPtr instruction;

    bool operator ==(const State& rhs) const { return (id == rhs.id) && (address == rhs.address); }
    bool isFromOperand() const { return index > -1; }
    const Operand* operand() const { return instruction->op(index); }
};

class StateMachine
{
    DEFINE_STATES(UserState = 0x10000000)

    protected:
        typedef std::function<void(const State*)> StateCallback;

    public:
        StateMachine();
        virtual ~StateMachine() = default;
        size_t pending() const;
        bool hasNext();
        void next();

    protected:
        void enqueueState(const State& state);
        void executeState(const State& state);
        void executeState(const State* state);
        virtual bool validateState(const State& state) const;
        virtual void onNewState(const State *state) const;

    private:
        bool getNext(State* state);

    protected:
        std::unordered_map<state_t, StateCallback> m_states;

    private:
        std::forward_list<State> m_pending;
        size_t m_count;
};

} // namespace REDasm

#endif // STATEMACHINE_H
