#pragma once

#include <redasm/plugins/plugin.h>

namespace REDasm {

class PluginImpl
{
    public:
        PluginImpl() = default;
        std::string id() const;
        const PluginDescriptor* descriptor() const;
        void setDescriptor(const PluginDescriptor* pd);

    private:
        const PluginDescriptor* m_descriptor;
};

} // namespace REDasm
