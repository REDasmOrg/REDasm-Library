#include "ordinals_impl.h"
#include <redasm/libs/json/json.hpp>
#include <redasm/support/utils.h>
#include <fstream>

namespace REDasm {

bool OrdinalsImpl::load(const std::string &filepath)
{
    std::ifstream ifs(filepath);

    if(!ifs.is_open())
        return false;

    nlohmann::json js;
    ifs >> js;

    for(auto it = js.begin(); it != js.end(); it++)
    {
        std::string ordinalstring = it.key(), name = it.value();

        if(ordinalstring.empty() || name.empty())
            continue;

        try
        {
            ordinal_t ordinal = static_cast<ordinal_t>(std::stoi(ordinalstring));
            m_ordinals[ordinal] = name;
        }
        catch(...)
        {
            continue;
        }
    }

    ifs.close();
    return true;
}

std::string OrdinalsImpl::ordinal(ordinal_t ordinal, const std::string &fallbackprefix) const
{
    auto it = m_ordinals.find(ordinal);

    if(it == m_ordinals.end())
        return fallbackprefix + "Ordinal__" + Utils::hex(ordinal, 16);

    return it->second;
}

void OrdinalsImpl::clear() { m_ordinals.clear(); }

} // namespace REDasm
