#include "referencetable.h"

void ReferenceTable::pushReference(rd_address address, rd_address refby) { m_references[address].insert(refby); }

void ReferenceTable::pushTarget(rd_address target, rd_address refby)
{
    this->pushReference(target, refby); // Targets are references too
    m_targets[refby].insert(target);
}

void ReferenceTable::popReference(rd_address target, rd_address refby)
{
    auto it = m_references.find(refby);
    if(it != m_references.end()) it->second.remove(target);
}

void ReferenceTable::popTarget(rd_address target, rd_address refby)
{
    this->popReference(target, refby);  // Targets are references too

    auto it = m_targets.find(refby);
    if(it != m_targets.end()) it->second.remove(target);
}

RDLocation ReferenceTable::target(rd_address address) const
{
    auto it = m_targets.find(address);
    if((it == m_targets.end()) || it->second.empty()) return { {0}, false };
    return { {it->second.front()}, true };
}

size_t ReferenceTable::targets(rd_address address, const rd_address** targets) const
{
    if(!targets) return 0;

    auto it = m_targets.find(address);
    if(it == m_targets.end()) return 0;
    return it->second.data(targets);
}

size_t ReferenceTable::references(rd_address address, const rd_address** references) const
{
    if(!references) return 0;

    auto it = m_references.find(address);
    if(it == m_references.end()) return 0;
    return it->second.data(references);
}

size_t ReferenceTable::referencesCount(rd_address address) const
{
    auto it = m_references.find(address);
    return (it != m_references.end()) ? it->second.size() : 0;
}

size_t ReferenceTable::targetsCount(rd_address address) const
{
    auto it = m_targets.find(address);
    return (it != m_targets.end()) ? it->second.size() : 0;
}
