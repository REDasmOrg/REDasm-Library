#pragma once

#include "string.h"
#include "../pimpl.h"

namespace REDasm {

class RegexImpl;
class RegexMatchIteratorImpl;

class RegexMatchIterator
{
    PIMPL_DECLARE_P(RegexMatchIterator)
    PIMPL_DECLARE_PRIVATE(RegexMatchIterator)

    public:
        struct Match { size_t start, end; String value; };

    private:
        RegexMatchIterator(const String& s, const String& pattern);

    public:
        bool hasNext() const;
        Match next();

    friend class RegexImpl;
};

class Regex
{
    PIMPL_DECLARE_P(Regex)
    PIMPL_DECLARE_PRIVATE(Regex)

    public:
        Regex(const String& pattern);
        bool search(const String& s) const;
        size_t match(const String& s) const;
        RegexMatchIterator matchAll(const String& s) const;
};

} // namespace REDasm
