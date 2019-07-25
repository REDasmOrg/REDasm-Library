#include "list_impl.h"

namespace REDasm {

Variant &ListImpl::first() { return m_list.front(); }
Variant &ListImpl::last() { return m_list.back(); };
const Variant &ListImpl::first() const { return m_list.front(); }
const Variant &ListImpl::last() const { return m_list.back(); }
Variant &ListImpl::at(size_t idx) { return m_list[idx]; }
const Variant &ListImpl::at(size_t idx) const { return m_list[idx]; }
size_t ListImpl::size() const { return m_list.size(); }

size_t ListImpl::indexOf(const Variant &v) const
{
    auto it = std::find(m_list.begin(), m_list.end(), v);
    return (it == m_list.end()) ? REDasm::npos : std::distance(m_list.begin(), it);
}

void ListImpl::append(const Variant &v) { m_list.push_back(v); }
void ListImpl::insert(size_t idx, const Variant &v) { m_list.insert(m_list.begin() + idx, v); }
void ListImpl::removeAt(size_t idx) { m_list.erase(m_list.begin() + idx); }

void ListImpl::remove(const Variant &v)
{
    auto it = std::find(m_list.begin(), m_list.end(), v);

    if(it != m_list.end())
        m_list.erase(it);
}

void ListImpl::releaseObjects() { ListImpl::releaseObjects(m_list); }
void ListImpl::clear() { m_list.clear(); }
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
