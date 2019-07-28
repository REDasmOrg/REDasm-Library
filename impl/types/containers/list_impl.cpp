#include "list_impl.h"

namespace REDasm {

void ListImpl::releaseObjects() { ListImpl::releaseObjects(m_list); }
void ListImpl::sort(const List::SortCallback &cb) { std::sort(m_list.begin(), m_list.end(), cb); }

Variant &SortedListImpl::first() { return m_list.front(); }
Variant &SortedListImpl::last() { return m_list.back(); }
Variant &SortedListImpl::at(size_t idx) { return m_list.at(idx); }
const Variant &SortedListImpl::first() const { return m_list.front(); }
const Variant &SortedListImpl::last() const { return m_list.back(); }
const Variant &SortedListImpl::at(size_t idx) const { return m_list.at(idx); }
size_t SortedListImpl::size() const { return m_list.size(); }
size_t SortedListImpl::indexOf(const Variant &obj) const { return m_list.indexOf(obj); }
size_t SortedListImpl::insertionIndex(const Variant &v) const { return m_list.insertionIndex(v); }
void SortedListImpl::insert(const Variant &v) { m_list.insert(v); }
void SortedListImpl::removeAt(size_t idx) { m_list.eraseAt(idx); }
void SortedListImpl::remove(const Variant &v) { m_list.erase(v); }
void SortedListImpl::releaseObjects() { ListImpl::releaseObjects(m_list); }
void SortedListImpl::clear() { m_list.clear(); }

} // namespace REDasm
