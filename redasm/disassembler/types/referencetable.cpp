#include "referencetable.h"
#include <algorithm>

namespace REDasm {

void ReferenceTable::push(address_t address, address_t refby)
{
    auto it = m_references.find(address);

    if(it == m_references.end())
    {
        m_references[address] = {refby};
        return;
    }

    it->second.insert(refby);
}

void ReferenceTable::pushTarget(address_t target, address_t pointedby)
{
    auto it = m_targets.find(pointedby);

    if(it == m_targets.end())
    {
        m_targets[pointedby] = {target};
        return;
    }

    it->second.insert(target);
}

void ReferenceTable::popTarget(address_t target, address_t pointedby)
{
    auto it = m_targets.find(pointedby);

    if(it == m_targets.end())
        return;

    it->second.erase(target);
}

ReferenceTable::ReferenceMap::const_iterator ReferenceTable::references(address_t address) const { return m_references.find(address); }

ReferenceSet ReferenceTable::targets(address_t address) const
{
    auto it = m_targets.find(address);

    if(it != m_targets.end())
        return it->second;

    return ReferenceSet();
}

address_location ReferenceTable::target(address_t address) const
{
    auto it = m_targets.find(address);

    if((it != m_targets.end()) && !it->second.empty())
        return REDasm::make_location(*it->second.begin());

    return REDasm::invalid_location<address_t>();
}

u64 ReferenceTable::referencesCount(address_t address) const
{
    auto it = this->references(address);

    if(it != m_references.end())
        return it->second.size();

    return 0;
}

u64 ReferenceTable::targetsCount(address_t address) const
{
    auto it = m_targets.find(address);

    if(it != m_targets.end())
        return it->second.size();

    return 0;
}

ReferenceVector ReferenceTable::referencesToVector(address_t address) const
{
    auto it = m_references.find(address);

    if(it == m_references.end())
        return ReferenceVector();

    return ReferenceTable::toVector(it->second);
}

ReferenceVector ReferenceTable::toVector(const ReferenceSet &refset)
{
    ReferenceVector rv;
    std::for_each(refset.begin(), refset.end(), [&rv](address_t address) { rv.push_back(address); });
    return rv;
}

}
