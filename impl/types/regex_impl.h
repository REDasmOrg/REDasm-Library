#pragma once

#include <redasm/types/regex.h>
#include <regex>

namespace REDasm {

class RegexMatchIteratorImpl
{
    public:
        RegexMatchIteratorImpl(const String& s, const String& pattern);
        bool hasNext() const;
        RegexMatch next();

    private:
        String m_string, m_pattern;
        std::sregex_token_iterator m_it;
        std::regex m_regex;
};

class RegexMatchListImpl
{
    public:
        RegexMatchListImpl();
        bool hasMatch() const;
        size_t size() const;
        RegexMatch at(size_t idx) const;

    private:
        std::smatch m_match;

    friend class RegexImpl;
};

class RegexImpl
{
    public:
        RegexImpl(const String& s);
        bool search(const String& pattern) const;
        RegexMatchList match(const String& s) const;
        RegexMatchIterator matchAll(const String& pattern) const;

    private:
        String m_pattern;
};

} // namespace REDasm
