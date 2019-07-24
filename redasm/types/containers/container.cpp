#include "container.h"

namespace REDasm {

bool Container::empty() const { return !this->size(); }
void ListContainerBase::removeFirst() { this->removeAt(0); }
void ListContainerBase::removeLast() { this->removeAt(this->size() - 1); }
const Variant &ListContainerBase::operator[](size_t idx) const { return this->at(idx); }
Variant &ListContainerBase::operator[](size_t idx) { return this->at(idx); }
const Variant &DictionaryContainer::operator[](const Variant &key) const { return this->value(key); }
Variant &DictionaryContainer::operator[](const Variant &key) { return this->value(key); }

} // namespace REDasm
