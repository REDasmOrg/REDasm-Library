#include "statemachine.h"
#include <iostream>

namespace REDasm {

StateMachine::StateMachine() { }

int StateMachine::concurrency() const { return m_concurrency; }

bool StateMachine::hasNext()
{
    state_lock lock(m_mutex);
    return !m_pending.empty();
}

void StateMachine::next()
{
    State currentstate;
    this->getNext(&currentstate);

    if(!(currentstate.id & StateMachine::UserState) && !this->validateState(currentstate))
        return;

    auto it = m_states.find(currentstate.id);

    if(it != m_states.end())
    {
        this->onNewState(currentstate);
        it->second(&currentstate);
    }
    else
        REDasm::log("Unknown state: " + std::to_string(currentstate.id));
}

void StateMachine::enqueueState(state_t state, u64 value, s64 index, const InstructionPtr &instruction)
{
    state_lock lock(m_mutex);
    m_pending.emplace(State{ state, static_cast<u64>(value), index, instruction });
}

bool StateMachine::validateState(const State &state) const
{
    RE_UNUSED(state);
    return true;
}

void StateMachine::onNewState(const State &state) const { RE_UNUSED(state); }

bool StateMachine::getNext(State *state)
{
    state_lock lock(m_mutex);

    if(m_pending.empty())
        return false;

    *state = m_pending.top();
    m_pending.pop();
    return true;
}

} // namespace REDasm
