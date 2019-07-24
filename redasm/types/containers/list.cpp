#include "list.h"
#include <impl/types/containers/list_impl.h>
#include <impl/libs/cereal/cereal.hpp>
#include <impl/libs/cereal/types/deque.hpp>
#include <impl/libs/cereal/archives/binary.hpp>

namespace REDasm {

List::List(): m_pimpl_p(new ListImpl()) { }
Variant &List::first() { PIMPL_P(List); return p->first(); }
Variant &List::last() { PIMPL_P(List); return p->last(); }
Variant &List::at(size_t idx) { PIMPL_P(List); return p->at(idx); }
const Variant &List::first() const { PIMPL_P(const List); return p->first(); }
const Variant &List::last() const { PIMPL_P(const List); return p->last(); }
const Variant &List::at(size_t idx) const { PIMPL_P(const List); return p->at(idx); }
size_t List::size() const { PIMPL_P(const List); return p->size(); }
size_t List::indexOf(const Variant& v) const { PIMPL_P(const List); return p->indexOf(v); }
void List::append(const Variant &v) { PIMPL_P(List); p->append(v); }
void List::insert(size_t idx, const Variant &v) { PIMPL_P(List); p->insert(idx, v); }
void List::removeAt(size_t idx) { PIMPL_P(List); p->remove(idx); }
void List::remove(const Variant &v) { PIMPL_P(List); p->remove(v); }
void List::sort(const SortCallback &cb) { PIMPL_P(List); return p->sort(cb); }
void List::clear() { PIMPL_P(List); return p->clear(); }
void List::save(cereal::BinaryOutputArchive &a) const { PIMPL_P(const List); a(p->m_list); }
void List::load(cereal::BinaryInputArchive &a) { PIMPL_P(List); a(p->m_list); }
void List::releaseObjects() { PIMPL_P(List); return p->releaseObjects(); }

} // namespace REDasm
