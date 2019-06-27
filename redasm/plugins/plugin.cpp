#include "plugin.h"
#include <impl/plugins/plugin_impl.h>

namespace REDasm {

const PluginDescriptor *Plugin::descriptor() const { PIMPL_P(const Plugin); return p->descriptor();  }
const PluginInstance *Plugin::instance() const { PIMPL_P(const Plugin); return p->instance();  }
void Plugin::setInstance(const PluginInstance *pi) { PIMPL_P(Plugin); p->setInstance(pi);  }
Plugin::Plugin(PluginImpl *p): m_pimpl_p(p) { }
Plugin::Plugin(): m_pimpl_p(new PluginImpl()) { }
String Plugin::id() const { PIMPL_P(const Plugin); return p->id(); }
String Plugin::description() const { PIMPL_P(const Plugin); return p->description(); }
int Plugin::weight() const { return 0; }

} // namespace REDasm
