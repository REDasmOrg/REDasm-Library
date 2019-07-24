#include "dictionary_impl.h"

namespace REDasm {

size_t DictionaryImpl::size() const { return m_dict.size(); }
const Variant &DictionaryImpl::value(const Variant &key) const { return m_dict.at(key); }
Variant &DictionaryImpl::value(const Variant &key) { return m_dict[key]; }
bool DictionaryImpl::contains(const Variant &key) const { return m_dict.find(key) != m_dict.end(); }
void DictionaryImpl::insert(const Variant &key, const Variant &value) { m_dict[key] = value; }
void DictionaryImpl::remove(const Variant &key) { m_dict.erase(key); }
void DictionaryImpl::clear() { m_dict.clear(); }

size_t SortedDictionaryImpl::size() const { return m_dict.size(); }
const Variant &SortedDictionaryImpl::value(const Variant &key) const { return m_dict.at(key); }
Variant &SortedDictionaryImpl::value(const Variant &key) { return m_dict[key]; }
bool SortedDictionaryImpl::contains(const Variant &key) const { return m_dict.find(key) != m_dict.end(); }
void SortedDictionaryImpl::insert(const Variant &key, const Variant &value) { m_dict[key] = value; }
void SortedDictionaryImpl::remove(const Variant &key) { m_dict.erase(key); }
void SortedDictionaryImpl::clear() { m_dict.clear(); }

} // namespace REDasm
