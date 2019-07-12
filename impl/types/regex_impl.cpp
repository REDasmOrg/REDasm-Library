#include "regex_impl.h"
#include "string_impl.h"
#include <algorithm>

namespace REDasm {

RegexMatchIteratorImpl::RegexMatchIteratorImpl(const String &s, const String &pattern): m_string(s), m_pattern(pattern), m_regex(pattern.pimpl_p()->m_data)
{
    m_it = std::sregex_token_iterator(m_string.pimpl_p()->m_data.begin(),
                                      m_string.pimpl_p()->m_data.end(),
                                      m_regex);
}

bool RegexMatchIteratorImpl::hasNext() const { return m_it != std::sregex_token_iterator(); }

RegexMatch RegexMatchIteratorImpl::next()
{
    RegexMatch m = { static_cast<size_t>(m_it->first - m_string.pimpl_p()->m_data.begin()),
                     static_cast<size_t>(m_it->second - m_string.pimpl_p()->m_data.begin() - 1),
                     m_it->str().c_str() };

    m_it++;
    return m;
}

RegexImpl::RegexImpl(const String &pattern): m_pattern(pattern) { }
bool RegexImpl::search(const String &s) const { return this->match(s).hasMatch(); }

RegexMatchList RegexImpl::match(const String &s) const
{
    std::smatch match;
    std::regex rgx(m_pattern.pimpl_p()->m_data);
    RegexMatchList ml;

    if(std::regex_search(s.pimpl_p()->m_data, match, rgx))
        ml.pimpl_p()->m_match = match;

    return ml;
}

RegexMatchIterator RegexImpl::matchAll(const String &s) const { return RegexMatchIterator(s, m_pattern); }

RegexMatchListImpl::RegexMatchListImpl() { }
bool RegexMatchListImpl::hasMatch() const { return !m_match.empty(); }
size_t RegexMatchListImpl::size() const { return m_match.size(); }

RegexMatch RegexMatchListImpl::at(size_t idx) const
{
    std::string s = m_match[idx];
    String v = s.c_str();

    return { static_cast<size_t>(m_match.position(idx)),
             static_cast<size_t>(m_match.position(idx) + v.size()),
             v};
}

} // namespace REDasm
