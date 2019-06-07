#include "referencetable.h"
#include <impl/disassembler/types/referencetable_impl.h>
#include <algorithm>

namespace REDasm {

ReferenceTable::ReferenceTable(): m_pimpl_p(new ReferenceTableImpl()) { }

void ReferenceTable::push(address_t address, address_t refby)
{
    PIMPL_P(ReferenceTable);
    auto it = p->m_references.find(address);

    if(it == p->m_references.end())
    {
        p->m_references[address] = {refby};
        return;
    }

    it->second.insert(refby);
}

void ReferenceTable::pushTarget(address_t target, address_t pointedby)
{
    PIMPL_P(ReferenceTable);
    auto it = p->m_targets.find(pointedby);

    if(it == p->m_targets.end())
    {
        p->m_targets[pointedby] = {target};
        return;
    }

    it->second.insert(target);
}

void ReferenceTable::popTarget(address_t target, address_t pointedby)
{
    PIMPL_P(ReferenceTable);
    auto it = p->m_targets.find(pointedby);

    if(it == p->m_targets.end())
        return;

    it->second.erase(target);
}

ReferenceTable::ReferenceMap::const_iterator ReferenceTable::references(address_t address) const { PIMPL_P(const ReferenceTable); return p->m_references.find(address); }

ReferenceSet ReferenceTable::targets(address_t address) const
{
    PIMPL_P(const ReferenceTable);
    auto it = p->m_targets.find(address);

    if(it != p->m_targets.end())
        return it->second;

    return ReferenceSet();
}

address_location ReferenceTable::target(address_t address) const
{
    PIMPL_P(const ReferenceTable);
    auto it = p->m_targets.find(address);

    if((it != p->m_targets.end()) && !it->second.empty())
        return REDasm::make_location(*it->second.begin());

    return REDasm::invalid_location<address_t>();
}

size_t ReferenceTable::referencesCount(address_t address) const
{
    PIMPL_P(const ReferenceTable);
    auto it = this->references(address);

    if(it != p->m_references.end())
        return it->second.size();

    return 0;
}

size_t ReferenceTable::targetsCount(address_t address) const
{
    PIMPL_P(const ReferenceTable);
    auto it = p->m_targets.find(address);

    if(it != p->m_targets.end())
        return it->second.size();

    return 0;
}

ReferenceVector ReferenceTable::referencesToVector(address_t address) const
{
    PIMPL_P(const ReferenceTable);
    auto it = p->m_references.find(address);

    if(it == p->m_references.end())
        return ReferenceVector();

    return ReferenceTable::toVector(it->second);
}

ReferenceVector ReferenceTable::toVector(const ReferenceSet &refset)
{
    ReferenceVector rv;
    std::for_each(refset.begin(), refset.end(), [&rv](address_t address) { rv.push_back(address); });
    return rv;
}

void Serializer<ReferenceTable>::write(std::fstream& fs, const ReferenceTable* t) {
    Serializer<ReferenceTable::ReferenceMap>::write(fs, t->pimpl_p()->m_references);
    Serializer<ReferenceTable::ReferenceMap>::write(fs, t->pimpl_p()->m_targets);
}

void Serializer<ReferenceTable>::read(std::fstream& fs, ReferenceTable* t) {
    Serializer<ReferenceTable::ReferenceMap>::read(fs, t->pimpl_p()->m_references);
    Serializer<ReferenceTable::ReferenceMap>::read(fs, t->pimpl_p()->m_targets);
}

}
