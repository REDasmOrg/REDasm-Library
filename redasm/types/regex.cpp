#include "regex.h"
#include <impl/types/regex_impl.h>

namespace REDasm {

Regex::Regex(const String &pattern): m_pimpl_p(new RegexImpl(pattern)) {  }
bool Regex::search(const String &s) const { PIMPL_P(const Regex); return p->search(s); }
size_t Regex::match(const String &s) const { PIMPL_P(const Regex); return p->match(s); }
RegexMatchIterator Regex::matchAll(const String &s) const { PIMPL_P(const Regex); return p->matchAll(s); }

RegexMatchIterator::RegexMatchIterator(const String &s, const String &pattern): m_pimpl_p(new RegexMatchIteratorImpl(s, pattern)) { }
bool RegexMatchIterator::hasNext() const { PIMPL_P(const RegexMatchIterator); return p->hasNext(); }
RegexMatchIterator::Match RegexMatchIterator::next() { PIMPL_P(RegexMatchIterator); return p->next(); }

} // namespace REDasm
