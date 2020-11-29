#include "documentnet.h"

void DocumentNet::linkSysCall(rd_address address, u64 syscall) { this->n(address).syscall = syscall; }
void DocumentNet::linkBranchIndirect(rd_address address) { this->n(address).branchtype = EmulateResult::BranchIndirect; }
void DocumentNet::linkBranchUnresolved(rd_address address) { this->n(address).branchtype = EmulateResult::BranchUnresolved; }

void DocumentNet::linkNext(rd_address fromaddress, rd_address toaddress)
{
    this->n(fromaddress).next = toaddress;
    this->n(toaddress).prev.insert(fromaddress);
}

void DocumentNet::unlinkNext(rd_address fromaddress)
{
    auto it = m_netnodes.find(fromaddress);
    if(it == m_netnodes.end()) return;

    rd_address toaddress = it->second.next;
    it->second.next = RD_NVAL;

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
            this->n(fromaddress).branchtype = EmulateResult::Branch;
            this->n(fromaddress).branchestrue.insert(toaddress);
            m_refs[toaddress].insert(fromaddress);
            break;

        case EmulateResult::BranchFalse:
            this->n(fromaddress).branchtype = EmulateResult::Branch;
            this->n(fromaddress).branchesfalse.insert(toaddress);
            break;

        case EmulateResult::BranchIndirect:
        case EmulateResult::BranchUnresolved:
            this->n(fromaddress).branchtype = type;
            break;

        default: return;
    }

    this->n(toaddress).from.insert(fromaddress);
}

void DocumentNet::unlinkBranch(rd_address fromaddress, rd_address toaddress)
{
    this->n(fromaddress).branchestrue.remove(toaddress);
    this->n(fromaddress).branchesfalse.remove(toaddress);
    this->n(toaddress).from.remove(fromaddress);
    m_refs[toaddress].remove(fromaddress);
}

void DocumentNet::linkCall(rd_address fromaddress, rd_address toaddress)
{
    this->n(fromaddress).calls.insert(toaddress);
    this->n(toaddress).from.insert(fromaddress);
    m_refs[toaddress].insert(fromaddress);
}

void DocumentNet::unlinkCall(rd_address fromaddress, rd_address toaddress)
{
    this->n(fromaddress).calls.remove(toaddress);
    this->n(toaddress).from.remove(fromaddress);
    m_refs[toaddress].remove(fromaddress);
}

void DocumentNet::addRef(rd_address fromaddress, rd_address toaddress) { m_refs[toaddress].insert(fromaddress); }
void DocumentNet::removeRef(rd_address fromaddress, rd_address toaddress) { m_refs[toaddress].remove(fromaddress); }

const DocumentNetNode* DocumentNet::findNode(rd_address address) const
{
    auto it = m_netnodes.find(address);
    return (it != m_netnodes.end()) ? std::addressof(it->second) : nullptr;
}

const DocumentNetNode* DocumentNet::prevNode(const DocumentNetNode* n) const
{
    if(!n || n->prev.empty()) return nullptr;
    return this->findNode(n->prev.front());
}

const DocumentNetNode* DocumentNet::nextNode(const DocumentNetNode* n) const
{
    if(!n || (n->next == RD_NVAL)) return nullptr;
    return this->findNode(n->next);
}

size_t DocumentNet::getReferences(rd_address address, const rd_address** refs) const
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

DocumentNetNode& DocumentNet::n(rd_address address)
{
    auto& nn = m_netnodes[address];
    nn.address = address;
    return nn;
}
