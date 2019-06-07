#include "plugin_impl.h"

namespace REDasm {

std::string PluginImpl::id() const { return m_descriptor->id; }
std::string PluginImpl::description() const { return m_descriptor->description; }
const PluginDescriptor *PluginImpl::descriptor() const { return m_descriptor; }
void PluginImpl::setDescriptor(const PluginDescriptor *pd) { m_descriptor = pd; }

} // namespace REDasm
