#include "context_impl.h"
#include <iostream>

namespace REDasm {

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
}

} // namespace REDasm
