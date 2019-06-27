#include "context_impl.h"
#include <iostream>
#include <capstone/capstone.h>

namespace REDasm {

std::unique_ptr<Context> ContextImpl::m_instance;
Context* ContextImpl::m_parentinstance = nullptr;

ContextImpl::ContextImpl(): m_disassembler(nullptr)
{
    m_laststatusreport = std::chrono::steady_clock::now();
    m_debouncetimeout = std::chrono::milliseconds(CONTEXT_DEBOUNCE_TIMEOUT_MS);
}

Disassembler *ContextImpl::disassembler() const { return m_disassembler; }
void ContextImpl::setDisassembler(Disassembler *disassembler) { m_disassembler = disassembler; }

void ContextImpl::checkSettings()
{
    if(!m_settings.logCallback)
        m_settings.logCallback = [](const String& s) { std::cout << s.c_str() << std::endl; };

    if(!m_settings.statusCallback)
        m_settings.statusCallback = [](const String& s) { };

    if(!m_settings.progressCallback)
        m_settings.progressCallback = [](size_t) { };

    if(!m_settings.ui)
        m_settings.ui = std::make_shared<AbstractUI>();
}

String ContextImpl::capstoneVersion() const
{
    int major = 0, minor = 0;
    cs_version(&major, &minor);
    return String::number(major) + "." + String::number(minor);
}

} // namespace REDasm
