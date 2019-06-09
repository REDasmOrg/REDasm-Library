#include "context_impl.h"
#include <iostream>
#include <capstone/capstone.h>

namespace REDasm {

std::unique_ptr<Context> ContextImpl::m_instance;
Context* ContextImpl::m_parentinstance = nullptr;

ContextImpl::ContextImpl()
{
    m_laststatusreport = std::chrono::steady_clock::now();
    m_debouncetimeout = std::chrono::milliseconds(CONTEXT_DEBOUNCE_TIMEOUT_MS);
}

void ContextImpl::checkSettings()
{
    if(!m_settings.logCallback)
        m_settings.logCallback = [](const std::string& s) { std::cout << s << std::endl; };

    if(!m_settings.statusCallback)
        m_settings.statusCallback = [](const std::string& s) { };

    if(!m_settings.progressCallback)
        m_settings.progressCallback = [](size_t) { };

    if(!m_settings.ui)
        m_settings.ui = std::make_shared<AbstractUI>();

    m_settings.pluginPaths.push_front(m_settings.runtimePath);
}

std::string ContextImpl::capstoneVersion() const
{
    int major = 0, minor = 0;
    cs_version(&major, &minor);
    return std::to_string(major) + "." + std::to_string(minor);
}

} // namespace REDasm
