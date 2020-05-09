#include "statemachine.h"
#include "../../document/document.h"
#include "../../support/utils.h"
#include "../../disassembler.h"
#include "../../context.h"
#include <rdapi/disassembler.h>

StateMachine::StateMachine(Disassembler* disassembler): m_disassembler(disassembler), m_document(disassembler->document()) { }
size_t StateMachine::pending() const { return m_pending.size(); }
bool StateMachine::hasNext() const { return !m_pending.empty(); }

void StateMachine::next()
{
    address_t address;
    if(!this->getNext(&address)) return;
    this->decodeAddress(address);
}

void StateMachine::enqueue(address_t address) { m_pending.emplace_front(address); }
void StateMachine::schedule(address_t address) { m_pending.emplace_back(address); }

bool StateMachine::getNext(address_t* address)
{
    if(m_pending.empty()) return false;

    *address = m_pending.front();
    m_pending.pop_front();
    return true;
}
