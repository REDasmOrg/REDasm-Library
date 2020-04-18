#include "statemachine.h"
#include "../../document/document.h"
#include "../../support/utils.h"
#include "../../disassembler.h"
#include "../../context.h"

size_t StateMachine::pending() const { return m_count; }

void StateMachine::executeState(const RDState* state)
{
    const auto& cb = m_states[state->id];

    if(cb)
    {
        this->onNewState(state);
        cb(state);
    }
    else
        rd_ctx->log("Unknown state: " + Utils::hex(state->id));
}

void StateMachine::executeState(const RDState& state) { this->executeState(&state); }
bool StateMachine::hasNext() const { return !m_pending.empty(); }

void StateMachine::next()
{
    RDState currentstate;
    if(!this->getNext(&currentstate)) return;
    this->executeState(&currentstate);
}

void StateMachine::enqueueState(const RDState& state)
{
    if(!this->validateState(&state)) return;
    m_pending.emplace_front(state);
    m_count++;
}

bool StateMachine::validateState(const RDState* state) const { return rd_doc->segment(state->address, nullptr); }
void StateMachine::onNewState(const RDState* state) const { }

bool StateMachine::getNext(RDState* state)
{
    if(m_pending.empty()) return false;

    *state = m_pending.front();
    m_pending.pop_front();
    m_count--;
    return true;
}
