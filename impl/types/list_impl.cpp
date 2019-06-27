#include "list_impl.h"

namespace REDasm {

Variant &ListImpl::first() { return m_list.front(); }
Variant &ListImpl::last() { return m_list.back(); };
const Variant &ListImpl::first() const { return m_list.front(); }
const Variant &ListImpl::last() const { return m_list.back(); }
Variant &ListImpl::at(size_t idx) { return m_list[idx]; }
const Variant &ListImpl::at(size_t idx) const { return m_list[idx]; }
size_t ListImpl::size() const { return m_list.size(); }

size_t ListImpl::indexOf(const Variant &obj) const
{
    auto it = std::find(m_list.begin(), m_list.end(), obj);
    return (it == m_list.end()) ? REDasm::npos : std::distance(m_list.begin(), it);
}

void ListImpl::append(const Variant &obj) { m_list.push_back(obj); }
void ListImpl::insert(size_t idx, const Variant &v) { m_list.insert(m_list.begin() + idx, v); }
void ListImpl::removeAt(size_t idx) { m_list.erase(m_list.begin() + idx); }

void ListImpl::remove(const Variant &v)
{
    auto it = std::find(m_list.begin(), m_list.end(), v);

    if(it != m_list.end())
        m_list.erase(it);
}

void ListImpl::removeFirst()
{
    if(m_list.empty())
        return;

    m_list.pop_front();
}

void ListImpl::removeLast()
{
    if(m_list.empty())
        return;

    m_list.pop_back();
}

void ListImpl::releaseObjects()
{
    for(auto it = m_list.begin(); it != m_list.end(); )
    {
        if(it->isObject())
        {
            Object* obj = variant_object<Object>(*it);
            it = m_list.erase(it);
            obj->release();
        }
        else
            it++;
    }
}

void ListImpl::sort(bool (*cb)(const Variant &, const Variant &)) { std::sort(m_list.begin(), m_list.end(), cb); }
bool ListImpl::empty() const { return m_list.empty(); }

ListIteratorImpl::ListIteratorImpl(List *list, size_t startidx): m_list(list), m_index(startidx) { }
bool ListIteratorImpl::hasNext() const { return m_index < m_list->size(); }
size_t ListIteratorImpl::index() const { return m_index - 1; }

Variant ListIteratorImpl::next()
{
    Variant v = m_list->at(m_index);
    m_index++;
    return v;
}

void ListIteratorImpl::remove() { m_list->removeAt(m_index); }

} // namespace REDasm
