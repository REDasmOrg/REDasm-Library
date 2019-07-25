#include "set_impl.h"

namespace REDasm {

size_t SetImpl::size() const { return m_list.size(); }
bool SetImpl::contains(const Variant &v) const { return m_set.find(v) != m_set.end(); }
void SetImpl::clear() { m_set.clear(); m_list.clear(); }
Variant &SetImpl::at(size_t idx) { return m_list[idx]; }
Variant &SetImpl::first() { return m_list.front(); }
Variant &SetImpl::last() { return m_list.back(); }
const Variant &SetImpl::at(size_t idx) const { return m_list.at(idx); }
const Variant &SetImpl::first() const { return m_list.front(); }
const Variant &SetImpl::last() const { return m_list.back(); }

size_t SetImpl::indexOf(const Variant &v) const
{
    auto it = std::find(m_list.begin(), m_list.end(), v);
    return (it == m_list.end()) ? REDasm::npos : std::distance(m_list.begin(), it);
}

void SetImpl::removeAt(size_t idx)
{
    m_set.erase(m_list[idx]);
    m_list.erase(m_list.begin() + idx);
}

void SetImpl::insert(const Variant &v)
{
    auto p = m_set.insert(v);

    if(p.second)
        m_list.push_back(v);
}

void SetImpl::remove(const Variant &v)
{
    m_set.erase(v);
    m_list.erase(std::remove(m_list.begin(), m_list.end(), v));
}

size_t SortedSetImpl::size() const { return m_list.size(); }
void SortedSetImpl::clear() { m_list.clear(); }
Variant &SortedSetImpl::at(size_t idx) { return m_list[idx]; }
Variant &SortedSetImpl::first() { return m_list.front(); }
Variant &SortedSetImpl::last() { return m_list.back(); }
const Variant &SortedSetImpl::at(size_t idx) const { return m_list.at(idx); }
const Variant &SortedSetImpl::first() const { return m_list.front(); }
const Variant &SortedSetImpl::last() const { return m_list.back(); }
size_t SortedSetImpl::indexOf(const Variant &v) const { return m_list.indexOf(v); }
void SortedSetImpl::removeAt(size_t idx) { m_list.eraseAt(idx); }
bool SortedSetImpl::contains(const Variant &v) const { return m_set.find(v) != m_set.end(); }

void SortedSetImpl::insert(const Variant &v)
{
    auto p = m_set.insert(v);

    if(p.second)
        m_list.insert(v);
}

void SortedSetImpl::remove(const Variant &v) { m_set.erase(v); m_list.erase(v); }

} // namespace REDasm
