#pragma once

#include "../types/string.h"
#include "../pimpl.h"

namespace REDasm {

typedef size_t ordinal_t;
class OrdinalsImpl;

class Ordinals
{
    PIMPL_DECLARE_P(Ordinals)
    PIMPL_DECLARE_PRIVATE(Ordinals)

    public:
        Ordinals();
        bool load(const String& filepath);
        String name(ordinal_t ordinal, const String& fallbackprefix = String()) const;
        void clear();

    public:
        static String ordinal(ordinal_t ord, const String& fallbackprefix = String());
};

} // namespace REDasm
