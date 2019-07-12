#include "regex.h"
#include <impl/types/regex_impl.h>

namespace REDasm {

Regex::Regex(const String &pattern): m_pimpl_p(new RegexImpl(pattern)) {  }
bool Regex::search(const String &s) const { PIMPL_P(const Regex); return p->search(s); }
RegexMatchList Regex::match(const String &s) const { PIMPL_P(const Regex); return p->match(s); }
RegexMatchIterator Regex::matchAll(const String &s) const { PIMPL_P(const Regex); return p->matchAll(s); }

RegexMatchIterator::RegexMatchIterator(const String &s, const String &pattern): m_pimpl_p(new RegexMatchIteratorImpl(s, pattern)) { }
bool RegexMatchIterator::hasNext() const { PIMPL_P(const RegexMatchIterator); return p->hasNext(); }
RegexMatch RegexMatchIterator::next() { PIMPL_P(RegexMatchIterator); return p->next(); }

RegexMatchList::RegexMatchList(): m_pimpl_p(new RegexMatchListImpl()) { }
bool RegexMatchList::hasMatch() const { PIMPL_P(const RegexMatchList); return p->hasMatch(); }
size_t RegexMatchList::size() const { PIMPL_P(const RegexMatchList); return p->size(); }
RegexMatch RegexMatchList::at(size_t idx) const { PIMPL_P(const RegexMatchList); return p->at(idx); }

} // namespace REDasm
