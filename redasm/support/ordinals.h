#pragma once

#include <redasm/types/string.h>
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
        String name(ordinal_t name, const String& fallbackprefix = String()) const;
        void clear();
};

} // namespace REDasm
