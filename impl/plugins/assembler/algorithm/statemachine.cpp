#include "statemachine.h"
#include <redasm/support/utils.h>
#include <redasm/context.h>

namespace REDasm {

StateMachine::StateMachine(): m_count(0) { }
size_t StateMachine::pending() const { return m_count; }
bool StateMachine::hasNext() const { return !m_pending.empty(); }

void StateMachine::next()
{
    State currentstate;

    if(!this->getNext(&currentstate))
        return;

    this->executeState(&currentstate);
}

void StateMachine::registerState(state_t id, const StateCallback &cb) { m_states[id] = cb; }

void StateMachine::enqueueState(const State& state)
{
    if(!state.isUser() && !this->validateState(state))
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
        r_ctx->log("Unknown state: " + Utils::hex(state->id));
}

bool StateMachine::validateState(const State &state) const
{
    return true;
}

void StateMachine::onNewState(const State *state) const { }

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
