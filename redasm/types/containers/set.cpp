#include "set.h"
#include <impl/types/containers/set_impl.h>

namespace REDasm {

Set::Set(): m_pimpl_p(new SetImpl()) { }
size_t Set::size() const { PIMPL_P(const Set); return p->size(); }
bool Set::contains(const Variant &v) const { PIMPL_P(const Set); return p->contains(v); }
void Set::insert(const Variant &v) { PIMPL_P(Set); p->insert(v); }
void Set::remove(const Variant &v) { PIMPL_P(Set); p->remove(v); }
void Set::clear() { PIMPL_P(Set); p->clear(); }

SortedSet::SortedSet(): m_pimpl_p(new SortedSetImpl()) { }
size_t SortedSet::size() const { PIMPL_P(const SortedSet); return p->size(); }
bool SortedSet::contains(const Variant &v) const { PIMPL_P(const SortedSet); return p->contains(v); }
void SortedSet::insert(const Variant &v) { PIMPL_P(SortedSet); p->insert(v); }
void SortedSet::remove(const Variant &v) { PIMPL_P(SortedSet); p->remove(v); }
void SortedSet::clear() { PIMPL_P(SortedSet); p->clear(); }

} // namespace REDasm
