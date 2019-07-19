#include "list.h"
#include <impl/types/list_impl.h>
#include <impl/libs/cereal/cereal.hpp>
#include <impl/libs/cereal/types/deque.hpp>
#include <impl/libs/cereal/archives/binary.hpp>

namespace REDasm {

List::List(): m_pimpl_p(new ListImpl()) { }
ListConstIterator List::iterator(size_t idx) const { return ListConstIterator(const_cast<List*>(this), idx); }
ListIterator List::iterator(size_t idx) { return ListIterator(this, idx); }
Variant &List::first() { PIMPL_P(List); return p->first(); }
Variant &List::last() { PIMPL_P(List); return p->last(); }
const Variant &List::first() const { PIMPL_P(const List); return p->first(); }
const Variant &List::last() const { PIMPL_P(const List); return p->last(); }
Variant &List::at(size_t idx) { PIMPL_P(List); return p->at(idx); }
const Variant &List::at(size_t idx) const { PIMPL_P(const List); return p->at(idx); }
size_t List::size() const { PIMPL_P(const List); return p->size(); }
size_t List::indexOf(const Variant& v) const { PIMPL_P(const List); return p->indexOf(v); }
void List::append(const Variant &v) { PIMPL_P(List); p->append(v); }
void List::insert(size_t idx, const Variant &v) { PIMPL_P(List); p->insert(idx, v); }
void List::removeAt(size_t idx) { PIMPL_P(List); p->remove(idx); }
void List::remove(const Variant &v) { PIMPL_P(List); p->remove(v); }
void List::removeFirst() { PIMPL_P(List); return p->removeFirst();  }
void List::removeLast() { PIMPL_P(List); return p->removeLast();  }
void List::sort(bool (*cb)(const Variant &, const Variant &)) { PIMPL_P(List); return p->sort(cb);   }
bool List::empty() const { PIMPL_P(const List); return p->empty(); }
Variant &List::operator[](size_t idx) { return this->at(idx);  }
const Variant &List::operator[](size_t idx) const { return this->at(idx); }
void List::save(cereal::BinaryOutputArchive &a) const { PIMPL_P(const List); a(p->m_list); }
void List::load(cereal::BinaryInputArchive &a) { PIMPL_P(List); a(p->m_list); }
void List::releaseObjects() { PIMPL_P(List); return p->releaseObjects();  }

ListIterator::ListIterator(List *list, size_t startidx): m_pimpl_p(new ListIteratorImpl(list, startidx)) { }
bool ListIterator::hasNext() const { PIMPL_P(const ListIterator); return p->hasNext(); }
size_t ListIterator::index() const { PIMPL_P(const ListIterator); return p->index(); }
Variant ListIterator::next() { PIMPL_P(ListIterator); return p->next(); }
void ListIterator::remove() { PIMPL_P(ListIterator); p->remove(); }

ListConstIterator::ListConstIterator(List *list, size_t startidx): ListIterator(list, startidx) { }


} // namespace REDasm
