#include "documentnet.h"

void DocumentNet::linkSysCall(rd_address address, u64 syscall) { m_netnodes[address].syscall = syscall; }
void DocumentNet::linkBranchIndirect(rd_address address) { m_netnodes[address].branchtype = EmulateResult::BranchIndirect; }
void DocumentNet::linkBranchUnresolved(rd_address address) { m_netnodes[address].branchtype = EmulateResult::BranchUnresolved; }

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

void DocumentNet::linkBranch(rd_address fromaddress, rd_address toaddress, rd_type type)
{
    switch(type)
    {
        case EmulateResult::Branch:
        case EmulateResult::BranchTrue:
            m_netnodes[fromaddress].branchtype = EmulateResult::Branch;
            m_netnodes[fromaddress].branchestrue.insert(toaddress);
            m_refs[toaddress].insert(fromaddress);
            break;

        case EmulateResult::BranchFalse:
            m_netnodes[fromaddress].branchtype = EmulateResult::Branch;
            m_netnodes[fromaddress].branchesfalse.insert(toaddress);
            break;

        case EmulateResult::BranchIndirect:
        case EmulateResult::BranchUnresolved:
            m_netnodes[fromaddress].branchtype = type;
            break;

        default: return;
    }

    m_netnodes[toaddress].from.insert(fromaddress);
}

void DocumentNet::unlinkBranch(rd_address fromaddress, rd_address toaddress)
{
    m_netnodes[fromaddress].branchestrue.remove(toaddress);
    m_netnodes[fromaddress].branchesfalse.remove(toaddress);
    m_netnodes[toaddress].from.remove(fromaddress);
    m_refs[toaddress].remove(fromaddress);
}

void DocumentNet::linkCall(rd_address fromaddress, rd_address toaddress)
{
    m_netnodes[fromaddress].calls.insert(toaddress);
    m_netnodes[toaddress].from.insert(fromaddress);
    m_refs[toaddress].insert(fromaddress);
}

void DocumentNet::unlinkCall(rd_address fromaddress, rd_address toaddress)
{
    m_netnodes[fromaddress].calls.remove(toaddress);
    m_netnodes[toaddress].from.remove(fromaddress);
    m_refs[toaddress].remove(fromaddress);
}

void DocumentNet::addRef(rd_address fromaddress, rd_address toaddress) { m_refs[toaddress].insert(fromaddress); }
void DocumentNet::removeRef(rd_address fromaddress, rd_address toaddress) { m_refs[toaddress].remove(fromaddress); }

const DocumentNetNode* DocumentNet::findNode(rd_address address) const
{
    auto it = m_netnodes.find(address);
    return (it != m_netnodes.end()) ? std::addressof(it->second) : nullptr;
}

size_t DocumentNet::getRefs(rd_address address, const rd_address** refs) const
{
    auto it = m_refs.find(address);
    if(it == m_refs.end()) return 0;
    return it->second.data(refs);
}

bool DocumentNet::isConditional(const DocumentNetNode* n) { return DocumentNet::isBranch(n) && (!n->branchestrue.empty() && !n->branchesfalse.empty()); }

bool DocumentNet::isBranch(const DocumentNetNode* n)
{
    switch(n->branchtype)
    {
        case EmulateResult::Branch:
        case EmulateResult::BranchTrue:
        case EmulateResult::BranchFalse:
        case EmulateResult::BranchIndirect:
        case EmulateResult::BranchUnresolved:
            return true;

        default: break;
    }

    return false;
}

bool DocumentNet::isCall(const DocumentNetNode* n) { return !n->calls.empty(); }
