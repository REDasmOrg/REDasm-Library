#include "documentnet.h"

void DocumentNet::linkNext(rd_address fromaddress, rd_address toaddress)
{
    m_netnodes[fromaddress].next = toaddress;
    m_netnodes[toaddress].prev.insert(fromaddress);
}

void DocumentNet::unlinkNext(rd_address fromaddress)
{
    auto it = m_netnodes.find(fromaddress);
    if(it == m_netnodes.end()) return;

    rd_address toaddress = it->second.next;
    it->second.next = RD_NPOS;

    it = m_netnodes.find(toaddress);
    if(it == m_netnodes.end()) return;

    it->second.prev.remove(fromaddress);
}

void DocumentNet::linkJump(rd_address fromddress, rd_address toaddress, bool condition)
{
    if(condition) m_netnodes[fromddress].truejumps.insert(toaddress);
    else m_netnodes[fromddress].falsejumps.insert(toaddress);
    m_netnodes[toaddress].from.insert(fromddress);
}

void DocumentNet::unlinkJump(rd_address fromaddress, rd_address toaddress)
{
    m_netnodes[fromaddress].truejumps.remove(toaddress);
    m_netnodes[fromaddress].falsejumps.remove(toaddress);
    m_netnodes[toaddress].from.remove(fromaddress);
}

void DocumentNet::linkCall(rd_address fromaddress, rd_address toaddress)
{
    m_netnodes[fromaddress].calls.insert(toaddress);
    m_netnodes[toaddress].from.insert(fromaddress);
}

void DocumentNet::unlinkCall(rd_address fromaddress, rd_address toaddress)
{
    m_netnodes[fromaddress].calls.remove(toaddress);
    m_netnodes[toaddress].from.remove(fromaddress);
}

const InstructionNetNode* DocumentNet::findNode(rd_address address) const
{
    auto it = m_netnodes.find(address);
    return (it != m_netnodes.end()) ? std::addressof(it->second) : nullptr;
}
