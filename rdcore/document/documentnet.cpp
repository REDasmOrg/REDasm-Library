#include "documentnet.h"

DocumentNet::DocumentNet(Context* ctx): Object(ctx) { }
void DocumentNet::linkSysCall(rd_address address, u64 syscall) { this->n(address).syscall = syscall; }
void DocumentNet::linkBranchIndirect(rd_address address) { this->n(address).branchtype = EmulateResult::BranchIndirect; }
void DocumentNet::linkBranchUnresolved(rd_address address) { this->n(address).branchtype = EmulateResult::BranchUnresolved; }

void DocumentNet::linkNext(rd_address fromaddress, rd_address toaddress)
{
    this->n(fromaddress).next = toaddress;
    this->n(toaddress).prev.insert(fromaddress);
}

bool DocumentNet::unlinkPrev(rd_address address)
{
    auto nit = m_netnodes.find(address);
    if(nit == m_netnodes.end()) return false;

    auto& n = nit->second;

    for(size_t i = 0; i < n.prev.size(); )
    {
        rd_address prevaddress = n.prev.at(i);
        auto pit = m_netnodes.find(prevaddress);

        if((pit != m_netnodes.end()) && (pit->second.next == address))
        {
            pit->second.next = RD_NVAL;
            n.prev.remove(prevaddress);
        }
        else
            i++;
    }

    return true;
}

bool DocumentNet::unlinkNext(rd_address address)
{
    auto it = m_netnodes.find(address);
    if(it == m_netnodes.end()) return false;

    rd_address nextaddress = it->second.next;
    if(nextaddress == RD_NVAL) return true;
    it->second.next = RD_NVAL;

    it = m_netnodes.find(nextaddress);
    if(it == m_netnodes.end()) return false;

    it->second.prev.remove(address);
    return true;
}

void DocumentNet::linkBranch(rd_address fromaddress, rd_address toaddress, rd_type type)
{
    switch(type)
    {
        case EmulateResult::Branch:
        case EmulateResult::BranchTrue:
            this->n(fromaddress).branchtype = EmulateResult::Branch;
            this->n(fromaddress).branchestrue.insert(toaddress);
            m_refs[toaddress].insert({ fromaddress, ReferenceFlags_Direct });
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
    this->removeRef(fromaddress, toaddress);
}

void DocumentNet::linkCall(rd_address fromaddress, rd_address toaddress)
{
    this->n(fromaddress).calls.insert(toaddress);
    this->n(toaddress).from.insert(fromaddress);
    m_refs[toaddress].insert({ fromaddress, ReferenceFlags_Direct });
}

void DocumentNet::unlinkCall(rd_address fromaddress, rd_address toaddress)
{
    this->n(fromaddress).calls.remove(toaddress);
    this->n(toaddress).from.remove(fromaddress);
    this->removeRef(fromaddress, toaddress);
}

void DocumentNet::addRef(rd_address fromaddress, rd_address toaddress, rd_flag flags) { m_refs[toaddress].insert({fromaddress, flags}); }

void DocumentNet::removeRef(rd_address fromaddress, rd_address toaddress)
{
    auto& refs = m_refs[toaddress];

    auto it = std::find_if(refs.begin(), refs.end(), [fromaddress](const RDReference& r) {
        return r.address == fromaddress;
    });

    if(it != refs.end()) refs.remove(*it);
}

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

size_t DocumentNet::getReferences(rd_address address, const RDReference** refs) const
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
