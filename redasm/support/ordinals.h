#pragma once

#include <string>
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
        bool load(const std::string& filepath);
        std::string ordinal(ordinal_t ordinal, const std::string& fallbackprefix = std::string()) const;
        void clear();
};

} // namespace REDasm
