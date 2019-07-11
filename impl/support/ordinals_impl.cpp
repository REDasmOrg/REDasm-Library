#include "ordinals_impl.h"
#include <redasm/libs/json/json.hpp>
#include <redasm/support/utils.h>
#include <fstream>

namespace REDasm {

bool OrdinalsImpl::load(const String &filepath)
{
    std::ifstream ifs(filepath.c_str());

    if(!ifs.is_open())
        return false;

    nlohmann::json js;
    ifs >> js;

    for(auto it = js.begin(); it != js.end(); it++)
    {
        String ordinalstring = it.key().c_str(), name = it.value();

        if(ordinalstring.empty() || name.empty())
            continue;

        try
        {
            ordinal_t ordinal = static_cast<ordinal_t>(ordinalstring.toInt());
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

String OrdinalsImpl::name(ordinal_t ord, const String &fallbackprefix) const
{
    auto it = m_ordinals.find(ord);

    if(it == m_ordinals.end())
        return ordinal(ord, fallbackprefix);

    return it->second;
}

void OrdinalsImpl::clear() { m_ordinals.clear(); }

String OrdinalsImpl::ordinal(ordinal_t ord, const String &fallbackprefix)
{
    return fallbackprefix + "Ordinal__" + String::number(ord, 16, 4, '0');
}

} // namespace REDasm
