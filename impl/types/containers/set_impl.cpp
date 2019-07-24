#include "set_impl.h"

namespace REDasm {

size_t SetImpl::size() const { return m_set.size(); }
bool SetImpl::contains(const Variant &v) const { return m_set.find(v) != m_set.end(); }
void SetImpl::insert(const Variant &v) { m_set.insert(v); }
void SetImpl::remove(const Variant &v) { m_set.erase(v); }
void SetImpl::clear() { m_set.clear(); }

size_t SortedSetImpl::size() const { return m_set.size(); }
bool SortedSetImpl::contains(const Variant &v) const { return m_set.find(v) != m_set.end(); }
void SortedSetImpl::insert(const Variant &v) { m_set.insert(v); }
void SortedSetImpl::remove(const Variant &v) { m_set.erase(v); }
void SortedSetImpl::clear() { m_set.clear(); }

} // namespace REDasm
