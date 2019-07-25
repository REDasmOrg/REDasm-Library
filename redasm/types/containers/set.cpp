#include "set.h"
#include <impl/types/containers/set_impl.h>

namespace REDasm {

Set::Set(): m_pimpl_p(new SetImpl()) { }
size_t Set::size() const { PIMPL_P(const Set); return p->size(); }
bool Set::contains(const Variant &v) const { PIMPL_P(const Set); return p->contains(v); }
void Set::insert(const Variant &v) { PIMPL_P(Set); p->insert(v); }
void Set::remove(const Variant &v) { PIMPL_P(Set); p->remove(v); }
void Set::clear() { PIMPL_P(Set); p->clear(); }
Variant &Set::at(size_t idx) { PIMPL_P(Set); return p->at(idx); }
Variant &Set::first() { PIMPL_P(Set); return p->first(); }
Variant &Set::last() { PIMPL_P(Set); return p->last(); }
const Variant &Set::at(size_t idx) const { PIMPL_P(const Set); return p->at(idx); }
const Variant &Set::first() const { PIMPL_P(const Set); return p->first(); }
const Variant &Set::last() const { PIMPL_P(const Set); return p->last(); }
size_t Set::indexOf(const Variant &v) const { PIMPL_P(const Set); return p->indexOf(v); }
void Set::removeAt(size_t idx) { PIMPL_P(Set); return p->remove(idx); }

SortedSet::SortedSet(): m_pimpl_p(new SortedSetImpl()) { }
size_t SortedSet::size() const { PIMPL_P(const SortedSet); return p->size(); }
bool SortedSet::contains(const Variant &v) const { PIMPL_P(const SortedSet); return p->contains(v); }
void SortedSet::insert(const Variant &v) { PIMPL_P(SortedSet); p->insert(v); }
void SortedSet::remove(const Variant &v) { PIMPL_P(SortedSet); p->remove(v); }
void SortedSet::clear() { PIMPL_P(SortedSet); p->clear(); }
Variant &SortedSet::at(size_t idx) { PIMPL_P(SortedSet); return p->at(idx); }
Variant &SortedSet::first() { PIMPL_P(SortedSet); return p->first(); }
Variant &SortedSet::last() { PIMPL_P(SortedSet); return p->last(); }
const Variant &SortedSet::at(size_t idx) const { PIMPL_P(const SortedSet); return p->at(idx); }
const Variant &SortedSet::first() const { PIMPL_P(const SortedSet); return p->first(); }
const Variant &SortedSet::last() const { PIMPL_P(const SortedSet); return p->last(); }
size_t SortedSet::indexOf(const Variant &v) const { PIMPL_P(const SortedSet); return p->indexOf(v); }
void SortedSet::removeAt(size_t idx) { PIMPL_P(SortedSet); return p->remove(idx); }

} // namespace REDasm
