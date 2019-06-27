#pragma once

#include <redasm/plugins/plugin.h>

namespace REDasm {

class PluginImpl
{
    public:
        PluginImpl() = default;
        virtual ~PluginImpl() = default;
        String id() const;
        String description() const;
        const PluginDescriptor* descriptor() const;
        const PluginInstance* instance() const;
        void setInstance(const PluginInstance* pd);

    private:
        const PluginInstance* m_plugininstance;
};

} // namespace REDasm
