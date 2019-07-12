#pragma once

#include "string.h"
#include "../pimpl.h"

namespace REDasm {

class RegexImpl;
class RegexMatchListImpl;
class RegexMatchIteratorImpl;

struct RegexMatch { size_t start, end; String value; };

class RegexMatchList
{
    PIMPL_DECLARE_P(RegexMatchList)
    PIMPL_DECLARE_PRIVATE(RegexMatchList)

    private:
        RegexMatchList();

    public:
        bool hasMatch() const;
        size_t size() const;
        RegexMatch at(size_t idx) const;

    friend class RegexImpl;
};

class RegexMatchIterator
{
    PIMPL_DECLARE_P(RegexMatchIterator)
    PIMPL_DECLARE_PRIVATE(RegexMatchIterator)

    private:
        RegexMatchIterator(const String& s, const String& pattern);

    public:
        bool hasNext() const;
        RegexMatch next();

    friend class RegexImpl;
};

class Regex
{
    PIMPL_DECLARE_P(Regex)
    PIMPL_DECLARE_PRIVATE(Regex)

    public:
        Regex(const String& pattern);
        bool search(const String& s) const;
        RegexMatchList match(const String& s) const;
        RegexMatchIterator matchAll(const String& s) const;
};

} // namespace REDasm
