#pragma once

#include <redasm/types/regex.h>
#include <regex>

namespace REDasm {

class RegexMatchIteratorImpl
{
    public:
        RegexMatchIteratorImpl(const String& s, const String& pattern);
        bool hasNext() const;
        RegexMatchIterator::Match next();

    private:
        String m_string, m_pattern;
        std::sregex_token_iterator m_it;
};

class RegexImpl
{
    public:
        RegexImpl(const String& s);
        bool search(const String& pattern) const;
        size_t match(const String& s) const;
        RegexMatchIterator matchAll(const String& pattern) const;

    private:
        String m_pattern;
};

} // namespace REDasm
