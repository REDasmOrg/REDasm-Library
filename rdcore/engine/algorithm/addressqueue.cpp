#include "addressqueue.h"
#include "../../document/document.h"
#include "../../support/utils.h"
#include "../../disassembler.h"
#include "../../context.h"

AddressQueue::AddressQueue(Context* ctx): m_disassembler(ctx->disassembler()), m_document(ctx->document()) { }
size_t AddressQueue::pending() const { return m_pending.size(); }
bool AddressQueue::hasNext() const { return !m_pending.empty(); }

void AddressQueue::next()
{
    rd_address address;
    if(!this->getNext(&address)) return;
    this->nextAddress(address);
}

void AddressQueue::enqueue(rd_address address) { m_pending.emplace_front(address); }
void AddressQueue::schedule(rd_address address) { m_pending.emplace_back(address); }

bool AddressQueue::getNext(rd_address* address)
{
    if(m_pending.empty()) return false;

    *address = m_pending.front();
    m_pending.pop_front();
    return true;
}
