#include "dictionary.h"
#include <impl/types/containers/dictionary_impl.h>

namespace REDasm {

Dictionary::Dictionary(): m_pimpl_p(new DictionaryImpl()) { }
size_t Dictionary::size() const { PIMPL_P(const Dictionary); return p->size(); }
const ListContainerBase &Dictionary::keys() const { PIMPL_P(const Dictionary); return p->keys(); }
const Variant &Dictionary::value(const Variant &key) const { PIMPL_P(const Dictionary); return p->value(key); }
Variant &Dictionary::value(const Variant &key) { PIMPL_P(Dictionary); return p->value(key); }
bool Dictionary::contains(const Variant &key) const { PIMPL_P(const Dictionary); return p->contains(key); }
void Dictionary::insert(const Variant &key, const Variant &value) { PIMPL_P(Dictionary); p->insert(key, value); }
void Dictionary::remove(const Variant &key) { PIMPL_P(Dictionary); p->remove(key); }
void Dictionary::clear() { PIMPL_P(Dictionary); p->clear(); }
const Variant &Dictionary::operator[](const Variant &key) const { return this->value(key); }
Variant &Dictionary::operator[](const Variant &key) { return this->value(key); }

SortedDictionary::SortedDictionary(): m_pimpl_p(new SortedDictionaryImpl()) { }
size_t SortedDictionary::size() const { PIMPL_P(const SortedDictionary); return p->size(); }
const ListContainerBase &SortedDictionary::keys() const { PIMPL_P(const SortedDictionary); return p->keys(); }
const Variant &SortedDictionary::value(const Variant &key) const { PIMPL_P(const SortedDictionary); return p->value(key); }
Variant &SortedDictionary::value(const Variant &key) { PIMPL_P(SortedDictionary); return p->value(key); }
bool SortedDictionary::contains(const Variant &key) const { PIMPL_P(const SortedDictionary); return p->contains(key); }
void SortedDictionary::insert(const Variant &key, const Variant &value) { PIMPL_P(SortedDictionary); return p->insert(key, value); }
void SortedDictionary::remove(const Variant &key) { PIMPL_P(SortedDictionary); return p->remove(key); }
void SortedDictionary::clear() { PIMPL_P(SortedDictionary); return p->clear(); }
const Variant &SortedDictionary::operator[](const Variant &key) const { return this->value(key); }
Variant &SortedDictionary::operator[](const Variant &key) { return this->value(key); }

} // namespace REDasm
