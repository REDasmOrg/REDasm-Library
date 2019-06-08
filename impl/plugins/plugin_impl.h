#pragma once

#include <redasm/plugins/plugin.h>

namespace REDasm {

class PluginImpl
{
    public:
        PluginImpl() = default;
        virtual ~PluginImpl() = default;
        std::string id() const;
        std::string description() const;
        const PluginDescriptor* descriptor() const;
        const PluginInstance* instance() const;
        void setInstance(const PluginInstance* pd);

    private:
        const PluginInstance* m_plugininstance;
};

} // namespace REDasm
