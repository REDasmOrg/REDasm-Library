#pragma once

#include <unordered_map>
#include <redasm/types/string.h>
#include <redasm/support/ordinals.h>

namespace REDasm {

class OrdinalsImpl
{
    PIMPL_DECLARE_Q(Ordinals)
    PIMPL_DECLARE_PUBLIC(Ordinals)

    private:
        typedef std::unordered_map<ordinal_t, String> OrdinalsMap;

    public:
        OrdinalsImpl() = default;
        bool load(const String& filepath);
        String name(ordinal_t ord, const String& fallbackprefix) const;
        void clear();

    public:
        static String ordinal(ordinal_t ord, const String& fallbackprefix);

    private:
        OrdinalsMap m_ordinals;
};

} // namespace REDasm
