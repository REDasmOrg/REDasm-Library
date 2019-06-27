#include "plugin_impl.h"

namespace REDasm {

String PluginImpl::id() const { return m_plugininstance->descriptor->id; }
String PluginImpl::description() const { return m_plugininstance->descriptor->description; }
const PluginDescriptor *PluginImpl::descriptor() const { return m_plugininstance->descriptor; }
const PluginInstance *PluginImpl::instance() const { return m_plugininstance; }
void PluginImpl::setInstance(const PluginInstance *pi) { m_plugininstance = pi; }

} // namespace REDasm
