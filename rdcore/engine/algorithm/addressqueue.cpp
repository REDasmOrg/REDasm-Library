#include "addressqueue.h"
#include "../../context.h"

AddressQueue::AddressQueue(Context* ctx): Object(ctx), m_document(ctx->document()), m_net(ctx->net()) { }
size_t AddressQueue::pending() const { return m_pending.size(); }
bool AddressQueue::hasNext() const { return !m_pending.empty(); }

void AddressQueue::next()
{
    rd_address address;
    if(!this->getNext(&address)) return;
    this->nextAddress(address);
}

void AddressQueue::enqueue(rd_address address) { m_pending.push_front(address); }
void AddressQueue::schedule(rd_address address) { m_pending.push_back(address); }

bool AddressQueue::getNext(rd_address* address)
{
    if(m_pending.empty()) return false;

    *address = m_pending.front();
    m_pending.pop_front();
    return true;
}
