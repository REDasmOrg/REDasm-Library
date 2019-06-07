#pragma once

#define CONTEXT_DEBOUNCE_TIMEOUT_MS 100

#include <redasm/context.h>
#include <chrono>
#include <deque>

namespace REDasm {

class ContextImpl
{
    PIMPL_DECLARE_PUBLIC(Context)

    public:
        ContextImpl();
        void checkSettings();

    private:
        ContextSettings m_settings;
        std::chrono::steady_clock::time_point m_laststatusreport;
        std::chrono::milliseconds m_debouncetimeout;
        std::set<std::string> m_uproblems;
        ProblemList m_problems;
};

} // namespace REDasm
