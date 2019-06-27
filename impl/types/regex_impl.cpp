#include "regex_impl.h"
#include "string_impl.h"
#include <algorithm>

namespace REDasm {

RegexMatchIteratorImpl::RegexMatchIteratorImpl(const String &s, const String &pattern): m_string(s), m_pattern(pattern)
{
    std::regex rgx(pattern.pimpl_p()->m_data);
    m_it = std::sregex_token_iterator(s.pimpl_p()->m_data.begin(),
                                      s.pimpl_p()->m_data.end(),
                                      rgx);
}

bool RegexMatchIteratorImpl::hasNext() const { return m_it != std::sregex_token_iterator(); }

RegexMatchIterator::Match RegexMatchIteratorImpl::next()
{
    RegexMatchIterator::Match m = { static_cast<size_t>(m_it->first - m_string.pimpl_p()->m_data.begin()),
                                    static_cast<size_t>(m_it->second - m_string.pimpl_p()->m_data.begin() - 1),
                                    m_it->str().c_str() };

    m_it++;
    return m;
}

RegexImpl::RegexImpl(const String &pattern): m_pattern(pattern) { }
bool RegexImpl::search(const String &s) const { return this->match(s) != REDasm::npos; }

size_t RegexImpl::match(const String &s) const
{
    std::smatch match;
    std::regex rgx(m_pattern.pimpl_p()->m_data);

    if(!std::regex_search(s.pimpl_p()->m_data, match, rgx))
        return REDasm::npos;

    return match.position();
}

RegexMatchIterator RegexImpl::matchAll(const String &s) const { return RegexMatchIterator(s, m_pattern); }

} // namespace REDasm
