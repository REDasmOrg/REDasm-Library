#include "plugin.h"
#include <impl/plugins/plugin_impl.h>

namespace REDasm {

const PluginDescriptor *Plugin::descriptor() const { PIMPL_P(const Plugin); return p->descriptor();  }
void Plugin::setDescriptor(const PluginDescriptor *pd) { PIMPL_P(Plugin); p->setDescriptor(pd);  }
Plugin::Plugin(PluginImpl *p): m_pimpl_p(p) { }
Plugin::Plugin(): m_pimpl_p(new PluginImpl()) { }
std::string Plugin::id() const { PIMPL_P(const Plugin); return p->id(); }
std::string Plugin::description() const { PIMPL_P(const Plugin); return p->description(); }

} // namespace REDasm
