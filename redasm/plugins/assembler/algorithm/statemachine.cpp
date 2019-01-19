#include "statemachine.h"
#include <iostream>

namespace REDasm {

StateMachine::StateMachine() { }
size_t StateMachine::pending() const { return m_pending.size(); }
bool StateMachine::hasNext() { return !m_pending.empty(); }

void StateMachine::next()
{
    State currentstate;

    if(!this->getNext(&currentstate))
        return;

    this->executeState(&currentstate);
}

void StateMachine::forwardState(const std::string &name, size_t id, u64 value, State *state)
{
    state->name = name;
    state->id = id;
    state->u_value = value;

    this->executeState(state);
}

void StateMachine::forwardState(const std::string &name, size_t id, State *state) { this->forwardState(name, id, state->u_value, state); }

void StateMachine::enqueueState(const State& state)
{
    if(!(state.id & StateMachine::UserState) && !this->validateState(state))
        return;

    m_pending.emplace(state);
}

void StateMachine::executeState(State state) { this->executeState(&state); }

void StateMachine::executeState(State *state)
{
    auto it = m_states.find(state->id);

    if(it != m_states.end())
    {
        this->onNewState(state);
        it->second(state);
    }
    else
        REDasm::log("Unknown state: " + REDasm::hex(state->id));
}

bool StateMachine::validateState(const State &state) const
{
    RE_UNUSED(state);
    return true;
}

void StateMachine::onNewState(const State *state) const { RE_UNUSED(state); }

bool StateMachine::getNext(State *state)
{
    if(m_pending.empty())
        return false;

    *state = m_pending.top();
    m_pending.pop();
    return true;
}

} // namespace REDasm
