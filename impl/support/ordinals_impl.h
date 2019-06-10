#pragma once

#include <unordered_map>
#include <string>
#include <redasm/support/ordinals.h>

namespace REDasm {

class OrdinalsImpl
{
    PIMPL_DECLARE_Q(Ordinals)
    PIMPL_DECLARE_PUBLIC(Ordinals)

    private:
        typedef std::unordered_map<ordinal_t, std::string> OrdinalsMap;

    public:
        OrdinalsImpl() = default;
        bool load(const std::string& filepath);
        std::string name(ordinal_t name, const std::string& fallbackprefix = std::string()) const;
        void clear();

    private:
        OrdinalsMap m_ordinals;
};

} // namespace REDasm
