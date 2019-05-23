#include "statemachine.h"
#include <iostream>

namespace REDasm {

StateMachine::StateMachine(): m_count(0) { }
size_t StateMachine::pending() const { return m_count; }
bool StateMachine::hasNext() { return !m_pending.empty(); }

void StateMachine::next()
{
    State currentstate;

    if(!this->getNext(&currentstate))
        return;

    this->executeState(&currentstate);
}

void StateMachine::enqueueState(const State& state)
{
    if(!(state.id & StateMachine::UserState) && !this->validateState(state))
        return;

    m_pending.emplace_front(state);
    m_count++;
}

void StateMachine::executeState(const State &state) { this->executeState(&state); }

void StateMachine::executeState(const State *state)
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

    *state = m_pending.front();
    m_pending.pop_front();
    m_count--;
    return true;
}

} // namespace REDasm
