#include "referencetable.h"

void ReferenceTable::pushReference(address_t address, address_t refby) { m_references[address].insert(refby); }
void ReferenceTable::pushTarget(address_t target, address_t refby) { m_targets[refby].insert(target); }

void ReferenceTable::popReference(address_t target, address_t refby)
{
    auto it = m_references.find(refby);
    if(it != m_references.end()) it->second.remove(target);
}

void ReferenceTable::popTarget(address_t target, address_t refby)
{
    auto it = m_targets.find(refby);
    if(it != m_targets.end()) it->second.remove(target);
}

RDLocation ReferenceTable::target(address_t address) const
{
    auto it = m_targets.find(address);
    if((it == m_targets.end()) || it->second.empty()) return { 0, false };
    return { it->second.front(), true };
}

size_t ReferenceTable::targets(address_t address, const address_t** targets) const
{
    if(!targets) return 0;

    auto it = m_targets.find(address);
    if(it == m_targets.end()) return 0;
    return it->second.data(targets);
}

size_t ReferenceTable::references(address_t address, const address_t** references) const
{
    if(!references) return 0;

    auto it = m_references.find(address);
    if(it == m_references.end()) return 0;
    return it->second.data(references);
}

size_t ReferenceTable::referencesCount(address_t address) const
{
    auto it = m_references.find(address);
    return (it != m_references.end()) ? it->second.size() : 0;
}

size_t ReferenceTable::targetsCount(address_t address) const
{
    auto it = m_targets.find(address);
    return (it != m_targets.end()) ? it->second.size() : 0;
}
