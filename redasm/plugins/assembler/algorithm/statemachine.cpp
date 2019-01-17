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

    auto it = m_states.find(currentstate.id);

    if(it != m_states.end())
    {
        this->onNewState(currentstate);
        it->second(&currentstate);
    }
    else
        REDasm::log("Unknown state: " + REDasm::hex(currentstate.id));
}

void StateMachine::enqueueState(const std::string &name, state_t id, u64 value, s64 index, const InstructionPtr &instruction)
{
    State state = { name, id, static_cast<u64>(value), index, instruction };

    if(!(id & StateMachine::UserState) && !this->validateState(state))
        return;

    m_pending.emplace(state);
}

bool StateMachine::validateState(const State &state) const
{
    RE_UNUSED(state);
    return true;
}

void StateMachine::onNewState(const State &state) const { RE_UNUSED(state); }

bool StateMachine::getNext(State *state)
{
    if(m_pending.empty())
        return false;

    *state = m_pending.top();
    m_pending.pop();
    return true;
}

} // namespace REDasm
